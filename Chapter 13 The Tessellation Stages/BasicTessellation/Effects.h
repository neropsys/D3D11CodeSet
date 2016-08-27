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
#pragma region TessellationEffect
class TessellationEffect : public Effect
{
public:
	TessellationEffect(
		ID3D11Device* device,
		const std::wstring& vertexShader, 
		const std::wstring& pixelShader,
		const std::wstring& domainShader,
		const std::wstring& hullShader);
	~TessellationEffect();
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
	ID3D11DomainShader* mDomainShader;
	ID3D11HullShader* mHullShader;
	ID3DBlob* mVSBlob;
	ID3DBlob* mPSBlob;
	ID3DBlob* mDSBlob;
	ID3DBlob* mHSBlob;
	ID3D11SamplerState* mSamplerState;
	ID3D11ShaderResourceView* mDiffuseMap;
};
#pragma endregion

#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();
	static TessellationEffect* TessellationFX;
};
#pragma endregion

#endif // EFFECTS_H