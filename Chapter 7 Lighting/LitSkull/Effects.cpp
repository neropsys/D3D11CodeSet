//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Effects.h"
#include <stdlib.h>
#include <d3dcompiler.h>
#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& vertexShader, const std::wstring& pixelShader)
	: mVertexShader(nullptr), mPixelShader(nullptr)
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

void Effect::SetEffect(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetShader(mPixelShader, NULL, 0);
	deviceContext->VSSetShader(mVertexShader, NULL, 0);
}


Effect::~Effect()
{
	ReleaseCOM(mPSBlob);
	ReleaseCOM(mVSBlob);
}
#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& vertexShader, const std::wstring& pixelShader)
	: Effect(device, vertexShader, pixelShader)
{
	mObjectConstantBuffer.Initialize(device);
	mFrameConstantBuffer.Initialize(device);
}

BasicEffect::~BasicEffect()
{

}
void BasicEffect::SetDirLights(const DirectionalLight* lights) {

	mFrameConstantBuffer.Data.mDirLights[0] = lights[0];
	mFrameConstantBuffer.Data.mDirLights[1] = lights[1];
	mFrameConstantBuffer.Data.mDirLights[2] = lights[2];


}
#pragma endregion

#pragma region Effects

BasicEffect* Effects::BasicFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX = new BasicEffect(device,L"SimpleVertexShader.cso" ,  L"SimplePixelShader.cso");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
}
#pragma endregion