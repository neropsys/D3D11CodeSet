//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& vertexShader, const std::wstring& pixelShader)
	: mVertexShader(nullptr), mPixelShader(nullptr), mPSBlob(nullptr), mVSBlob(nullptr), mGSBlob(nullptr)
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
}

Effect::~Effect()
{
	ReleaseCOM(mPSBlob);
	ReleaseCOM(mVSBlob);
	ReleaseCOM(mGSBlob);
	ReleaseCOM(mSamplerState);
}

void Effect::SetEffect(ID3D11DeviceContext* deviceContext)
{
	//disable geometry shader from drawing tree billboard
	deviceContext->GSSetShader(0, NULL, 0);

	deviceContext->PSSetShader(mPixelShader, NULL, 0);
	deviceContext->VSSetShader(mVertexShader, NULL, 0);
	deviceContext->PSSetShaderResources(0, 1, &mDiffuseMap);
	deviceContext->PSSetSamplers(0, 1, &mSamplerState);
}

#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& vertexShader, const std::wstring& pixelShader)
	:Effect(device, vertexShader, pixelShader)
{
	mObjectConstantBuffer.Initialize(device);
	mFrameConstantBuffer.Initialize(device);
}

BasicEffect::~BasicEffect()
{
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

#pragma endregion

#pragma region TreeSpriteEffect
TreeSpriteEffect::TreeSpriteEffect(ID3D11Device* device, const std::wstring& vertexShader, const std::wstring& pixelShader, const std::wstring& geometryShader)
	: Effect(device, vertexShader, pixelShader)
{
	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	//convert wchar_t to char*
	size_t size = wcslen(geometryShader.c_str()) * 2 + 2;
	char* fileName = new char[size];
	size_t c_size;
	wcstombs_s(&c_size, fileName, size, geometryShader.c_str(), size);

	// Load cso files and create shaders
	HR(ShaderHelper::LoadCompiledShader(fileName, &mGSBlob));
	HR(device->CreateGeometryShader(mGSBlob->GetBufferPointer(), mGSBlob->GetBufferSize(), NULL, &mGeometryShader));

	delete fileName;
	fileName = nullptr;
	mObjectConstantBuffer.Initialize(device);
	mFrameConstantBuffer.Initialize(device);
}

TreeSpriteEffect::~TreeSpriteEffect()
{
}

void TreeSpriteEffect::SetEffect(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetShader(mPixelShader, NULL, 0);
	deviceContext->VSSetShader(mVertexShader, NULL, 0);
	deviceContext->PSSetShaderResources(0, 1, &mDiffuseMap);
	deviceContext->PSSetSamplers(0, 1, &mSamplerState);

	deviceContext->GSSetShader(mGeometryShader, NULL, 0);
}

void TreeSpriteEffect::SetDirLights(const DirectionalLight* lights)
{
	mFrameConstantBuffer.Data.mDirLights[0] = lights[0];
	mFrameConstantBuffer.Data.mDirLights[1] = lights[1];
	mFrameConstantBuffer.Data.mDirLights[2] = lights[2];
}

void TreeSpriteEffect::ApplyChanges(ID3D11DeviceContext* deviceContext)
{
	mFrameConstantBuffer.ApplyChanges(deviceContext);
	mObjectConstantBuffer.ApplyChanges(deviceContext);

	ID3D11Buffer* buffer[2] = { mObjectConstantBuffer.Buffer(), mFrameConstantBuffer.Buffer() };

	deviceContext->PSSetConstantBuffers(0, 2, buffer);
	deviceContext->GSSetConstantBuffers(0, 2, buffer);
}

#pragma endregion

#pragma region Effects

BasicEffect*      Effects::BasicFX      = 0;
TreeSpriteEffect* Effects::TreeSpriteFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX = new BasicEffect(device, L"SimpleVertexShader.cso", L"SimplePixelShader.cso");
	TreeSpriteFX = new TreeSpriteEffect(device, L"TreeVertexShader.cso", L"TreePixelShader.cso", L"TreeGeometryShader.cso");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(TreeSpriteFX);
}
#pragma endregion