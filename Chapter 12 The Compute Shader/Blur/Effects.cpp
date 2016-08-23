//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device)
{
	
}

void Effect::SetEffect(ID3D11DeviceContext* deviceContext)
{
}

Effect::~Effect()
{


}
#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& vertexShader, const std::wstring& pixelShader)
	:Effect(device)
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	//convert wchar_t to char*
	size_t size = wcslen(pixelShader.c_str()) * 2 + 2;
	char* fileName = new char[size];
	size_t c_size;
	wcstombs_s(&c_size, fileName, size, pixelShader.c_str(), size);

	// Load cso files and create shaders
	HR(ShaderHelper::LoadCompiledShader(fileName, &mPSBlob));
	HR(device->CreatePixelShader(mPSBlob->GetBufferPointer(), mPSBlob->GetBufferSize(), NULL, &mPixelShader));

	delete fileName;

	size = wcslen(vertexShader.c_str()) * 2 + 2;
	fileName = new char[size];
	wcstombs_s(&c_size, fileName, size, vertexShader.c_str(), size);

	HR(ShaderHelper::LoadCompiledShader(fileName, &mVSBlob));
	HR(device->CreateVertexShader(mVSBlob->GetBufferPointer(), mVSBlob->GetBufferSize(), NULL, &mVertexShader));
	delete fileName;
	fileName = nullptr;
	mObjectConstantBuffer.Initialize(device);
	mFrameConstantBuffer.Initialize(device);
}

BasicEffect::~BasicEffect()
{
	ReleaseCOM(mPSBlob);
	ReleaseCOM(mVSBlob);
	ReleaseCOM(mSamplerState);
}

void BasicEffect::SetDirLights(const DirectionalLight* lights)
{
	mFrameConstantBuffer.Data.mDirLights[0] = lights[0];
	mFrameConstantBuffer.Data.mDirLights[1] = lights[1];
	mFrameConstantBuffer.Data.mDirLights[2] = lights[2];
}

void BasicEffect::ApplyChanges(ID3D11DeviceContext* deviceContext)
{
	mFrameConstantBuffer.ApplyChanges(deviceContext);
	mObjectConstantBuffer.ApplyChanges(deviceContext);

	ID3D11Buffer* buffer[2] = { mObjectConstantBuffer.Buffer(), mFrameConstantBuffer.Buffer() };

	deviceContext->VSSetConstantBuffers(0, 1, &buffer[0]);
	deviceContext->PSSetConstantBuffers(0, 2, buffer);
}

void BasicEffect::SetEffect(ID3D11DeviceContext* deviceContext)
{

	deviceContext->PSSetShader(mPixelShader, NULL, 0);
	deviceContext->VSSetShader(mVertexShader, NULL, 0);
	deviceContext->PSSetShaderResources(0, 1, &mDiffuseMap);
	deviceContext->PSSetSamplers(0, 1, &mSamplerState);
}

#pragma endregion

#pragma region BlurEffect
BlurEffect::BlurEffect(ID3D11Device* device,
	const std::wstring& horzBlurCShader,
	const std::wstring& vertBlurCShader)
	: Effect(device)
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	//convert wchar_t to char*
	size_t size = wcslen(vertBlurCShader.c_str()) * 2 + 2;
	char* fileName = new char[size];
	size_t c_size;
	wcstombs_s(&c_size, fileName, size, vertBlurCShader.c_str(), size);

	// Load cso files and create shaders
	HR(ShaderHelper::LoadCompiledShader(fileName, &mVCSBlob));
	HR(device->CreateComputeShader(mVCSBlob->GetBufferPointer(), mVCSBlob->GetBufferSize(), NULL, &mVertComputeShader));

	delete fileName;
	fileName = nullptr;
	//convert wchar_t to char*
	size = wcslen(horzBlurCShader.c_str()) * 2 + 2;
	fileName = new char[size];
	wcstombs_s(&c_size, fileName, size, horzBlurCShader.c_str(), size);

	// Load cso files and create shaders
	HR(ShaderHelper::LoadCompiledShader(fileName, &mHCSBlob));
	HR(device->CreateComputeShader(mHCSBlob->GetBufferPointer(), mHCSBlob->GetBufferSize(), NULL, &mHorizComputeShader));

	delete fileName;
	fileName = nullptr;

	mSettingBuffer.Initialize(device);
}

BlurEffect::~BlurEffect()
{
	ReleaseCOM(mHCSBlob);
	ReleaseCOM(mVCSBlob);
}

void BlurEffect::SetVertEffect(ID3D11DeviceContext * deviceContext)
{
	
	deviceContext->CSSetShader(mVertComputeShader, NULL, 0);
}

void BlurEffect::SetHorzEffect(ID3D11DeviceContext* deviceContext)
{
	
	deviceContext->CSSetShader(mHorizComputeShader, NULL, 0);
}

void BlurEffect::SetWeights(const float weights[9])
{
	for (int i = 0; i < 9; i++) {
		mSettingBuffer.Data.mWeights[i] = weights[i];
	}
}

void BlurEffect::ApplyHorzChanges(ID3D11DeviceContext* deviceContext)
{
	mSettingBuffer.ApplyChanges(deviceContext);

	//ID3D11Buffer* buffer = mSettingBuffer.Buffer();
	//deviceContext->CSSetConstantBuffers(2, 1, &buffer);
	deviceContext->CSSetShaderResources(0, 1, &HorzInputMap);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &HorzOutputMap, 0);
}

void BlurEffect::ApplyVertChanges(ID3D11DeviceContext* deviceContext)
{
	mSettingBuffer.ApplyChanges(deviceContext);

	//ID3D11Buffer* buffer = mSettingBuffer.Buffer();
	//deviceContext->CSSetConstantBuffers(2, 1, &buffer);
	deviceContext->CSSetShaderResources(0, 1, &VertInputMap);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &VertOutputMap, 0);
}


#pragma endregion

#pragma region Effects

BasicEffect*      Effects::BasicFX      = 0;
BlurEffect*       Effects::BlurFX       = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX = new BasicEffect(device, L"SimpleVertexShader.cso", L"SimplePixelShader.cso");
	BlurFX = new BlurEffect(device, L"HorzBlurComputeShader.cso", L"VertBlurComputeShader.cso");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(BlurFX);
}
#pragma endregion