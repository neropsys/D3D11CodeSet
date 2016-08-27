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

TessellationEffect* Effects::TessellationFX = 0;

#pragma region Effects
void Effects::InitAll(ID3D11Device* device)
{
	TessellationFX = new TessellationEffect(
		device,
		L"TessVertexShader.cso",
		L"TessPixelShader.cso",
		L"TessDomainShader.cso",
		L"TessHullShader.cso");
}

void Effects::DestroyAll()
{
	SafeDelete(TessellationFX);
}
#pragma endregion

#pragma region TessellationEffect

TessellationEffect::TessellationEffect(
	ID3D11Device* device,
	const std::wstring& vertexShader,
	const std::wstring& pixelShader,
	const std::wstring& domainShader,
	const std::wstring& hullShader)
	:Effect(device),
	mPSBlob(0),
	mVSBlob(0),
	mDSBlob(0),
	mHSBlob(0),

	mHullShader(0),
	mDomainShader(0),
	mVertexShader(0),
	mPixelShader(0),
	mSamplerState(0)
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	//create pixel shader
	//convert wchar_t to char*
	size_t size = wcslen(pixelShader.c_str()) * 2 + 2;
	char* fileName = new char[size];
	size_t c_size;
	wcstombs_s(&c_size, fileName, size, pixelShader.c_str(), size);

	// Load cso files and create shaders
	HR(ShaderHelper::LoadCompiledShader(fileName, &mPSBlob));
	HR(device->CreatePixelShader(mPSBlob->GetBufferPointer(), mPSBlob->GetBufferSize(), NULL, &mPixelShader));

	delete fileName;

	//create vertex shader
	size = wcslen(vertexShader.c_str()) * 2 + 2;
	fileName = new char[size];
	wcstombs_s(&c_size, fileName, size, vertexShader.c_str(), size);

	HR(ShaderHelper::LoadCompiledShader(fileName, &mVSBlob));
	HR(device->CreateVertexShader(mVSBlob->GetBufferPointer(), mVSBlob->GetBufferSize(), NULL, &mVertexShader));
	delete fileName;
	fileName = nullptr;

	//create domain shader
	size = wcslen(domainShader.c_str()) * 2 + 2;
	fileName = new char[size];
	wcstombs_s(&c_size, fileName, size, domainShader.c_str(), size);

	HR(ShaderHelper::LoadCompiledShader(fileName, &mDSBlob));
	HR(device->CreateDomainShader(mDSBlob->GetBufferPointer(), mDSBlob->GetBufferSize(), NULL, &mDomainShader));
	delete fileName;
	fileName = nullptr;

	//create hull shader
	size = wcslen(hullShader.c_str()) * 2 + 2;
	fileName = new char[size];
	wcstombs_s(&c_size, fileName, size, hullShader.c_str(), size);

	HR(ShaderHelper::LoadCompiledShader(fileName, &mHSBlob));
	HR(device->CreateHullShader(mHSBlob->GetBufferPointer(), mHSBlob->GetBufferSize(), NULL, &mHullShader));
	delete fileName;
	fileName = nullptr;


	mObjectConstantBuffer.Initialize(device);
	mFrameConstantBuffer.Initialize(device);
}

TessellationEffect::~TessellationEffect()
{
	ReleaseCOM(mPSBlob);
	ReleaseCOM(mVSBlob);
	ReleaseCOM(mHSBlob);
	ReleaseCOM(mDSBlob);
	ReleaseCOM(mSamplerState);
}

void TessellationEffect::SetDirLights(const DirectionalLight* lights)
{
	mFrameConstantBuffer.Data.mDirLights[0] = lights[0];
	mFrameConstantBuffer.Data.mDirLights[1] = lights[1];
	mFrameConstantBuffer.Data.mDirLights[2] = lights[2];
}

void TessellationEffect::ApplyChanges(ID3D11DeviceContext* deviceContext)
{
	mFrameConstantBuffer.ApplyChanges(deviceContext);
	mObjectConstantBuffer.ApplyChanges(deviceContext);

	ID3D11Buffer* buffer[2] = { mObjectConstantBuffer.Buffer(), mFrameConstantBuffer.Buffer() };

	deviceContext->DSSetConstantBuffers(0, 1, &buffer[0]);
	deviceContext->HSSetConstantBuffers(0, 2, buffer);


}

void TessellationEffect::SetEffect(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetShader(mPixelShader, NULL, 0);
	deviceContext->VSSetShader(mVertexShader, NULL, 0);
	deviceContext->HSSetShader(mHullShader, NULL, 0);
	deviceContext->DSSetShader(mDomainShader, NULL, 0);
}

#pragma endregion