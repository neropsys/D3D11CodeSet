//***************************************************************************************
// Effects.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines lightweight effect wrappers to group an effect and its variables.
// Also defines a static Effects class from which we can access all of our effects.
//***************************************************************************************

#ifndef EFFECTS_H
#define EFFECTS_H

#include "d3dUtil.h"
#include "ConstantBuffer.h"
#include "cbPerFrame.h"
#include "cbPerObject.h"
#include "cbSettings.h"
#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* device);
	virtual ~Effect();
	virtual void SetEffect(ID3D11DeviceContext* deviceContext);
private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);
	
};
#pragma endregion

#pragma region BasicEffect
class BasicEffect : public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& vertexShader, const std::wstring& pixelShader);
	~BasicEffect();

	void SetWorldViewProj(CXMMATRIX M) { XMStoreFloat4x4(&mObjectConstantBuffer.Data.mWorldViewProj, M); }
	void SetWorld(CXMMATRIX M) { XMStoreFloat4x4(&mObjectConstantBuffer.Data.mWorld, M); }
	void SetWorldInvTranspose(CXMMATRIX M) { XMStoreFloat4x4(&mObjectConstantBuffer.Data.mWorldInvTranspose, M); }
	void SetTexTransform(CXMMATRIX M) { XMStoreFloat4x4(&mObjectConstantBuffer.Data.mTexTransform, M); }
	void SetEyePosW(const XMFLOAT3& v) { mFrameConstantBuffer.Data.mEyePosW = v; }
	void SetSamplerState(ID3D11SamplerState* samplerState) { mSamplerState = samplerState; }
	void SetFogColor(const FXMVECTOR v) { XMStoreFloat4(&mFrameConstantBuffer.Data.mFogColor, v); }
	void SetFogStart(float f) { mFrameConstantBuffer.Data.mFogStart = f; }
	void SetFogRange(float f) { mFrameConstantBuffer.Data.mFogRange = f; }
	void SetDirLights(const DirectionalLight* lights);
	void SetMaterial(const Material& mat) { mObjectConstantBuffer.Data.mMaterial = mat; }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex) { mDiffuseMap = tex; }
	void ApplyChanges(ID3D11DeviceContext* deviceContext);
	void SetEffect(ID3D11DeviceContext* deviceContext)override;

	inline ID3DBlob* getVSBlob() { return mVSBlob; }
	inline ID3DBlob* getPSBlob() { return mPSBlob; }
	ConstantBuffer<cbPerObject> mObjectConstantBuffer;
	ConstantBuffer<cbPerFrame> mFrameConstantBuffer;

	ID3D11PixelShader* mPixelShader;
	ID3D11VertexShader* mVertexShader;
	ID3DBlob* mVSBlob;

	ID3DBlob* mPSBlob;
	ID3D11SamplerState* mSamplerState;
	ID3D11ShaderResourceView* mDiffuseMap;
};
#pragma endregion

#pragma region BlurEffect
class BlurEffect : public Effect
{
public:
	BlurEffect(ID3D11Device* device, const std::wstring& horzBlurCShader, const std::wstring& vertBlurCShader);
	~BlurEffect();
	void SetVertEffect(ID3D11DeviceContext* deviceContext);
	void SetHorzEffect(ID3D11DeviceContext* deviceContext);
	void SetWeights(const float weights[9]);
	void SetHorzInputMap(ID3D11ShaderResourceView* tex) { HorzInputMap = tex; }
	void SetVertInputMap(ID3D11ShaderResourceView* tex) { VertInputMap = tex; }
	void SetHorzOutputMap(ID3D11UnorderedAccessView* tex) { HorzOutputMap = tex; }
	void SetVertOutputMap(ID3D11UnorderedAccessView* tex) { VertOutputMap = tex; }
	void ApplyHorzChanges(ID3D11DeviceContext* deviceContext);
	void ApplyVertChanges(ID3D11DeviceContext* deviceContext);
	ConstantBuffer<cbSettings> mSettingBuffer;
	
	ID3D11ShaderResourceView* HorzInputMap;
	ID3D11UnorderedAccessView* HorzOutputMap;

	ID3D11ShaderResourceView* VertInputMap;
	ID3D11UnorderedAccessView* VertOutputMap;

	ID3D11ComputeShader* mHorizComputeShader;
	ID3D11ComputeShader* mVertComputeShader;
	ID3DBlob* mHCSBlob;
	ID3DBlob* mVCSBlob;
};
#pragma endregion

#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect* BasicFX;
	static BlurEffect* BlurFX;
};
#pragma endregion

#endif // EFFECTS_H