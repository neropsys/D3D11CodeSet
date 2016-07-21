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
	Effect(ID3D11Device* device, const std::wstring& vertexShader, const std::wstring& pixelShader);
	void SetEffect(ID3D11DeviceContext* deviceContext);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3D11PixelShader* mPixelShader;
	ID3D11VertexShader* mVertexShader;
	ID3DBlob* mVSBlob;
	ID3DBlob* mPSBlob;
};
#pragma endregion

#pragma region BasicEffect
class BasicEffect : public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& vertexShader, const std::wstring& pixelShader);
	~BasicEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { XMStoreFloat4x4(&mObjectConstantBuffer.Data.mWorldViewProj,M); }
	void SetWorld(CXMMATRIX M)                          { XMStoreFloat4x4(&mObjectConstantBuffer.Data.mWorld, M); }
	void SetWorldInvTranspose(CXMMATRIX M)				{ XMStoreFloat4x4(&mObjectConstantBuffer.Data.mWorldInvTranspose, M); }
	void SetEyePosW(const XMFLOAT3& v)					{ mFrameConstantBuffer.Data.mEyePosW = v; }
	void SetDirLights(const DirectionalLight* lights);

	
	void SetMaterial(const Material& mat)				{ mObjectConstantBuffer.Data.mMaterial = mat;}

	inline ID3DBlob* getVSBlob() { return mVSBlob; }
	inline ID3DBlob* getPSBlob() { return mPSBlob; }
	ConstantBuffer<cbPerObject> mObjectConstantBuffer;
	ConstantBuffer<cbPerFrame> mFrameConstantBuffer;


};
#pragma endregion

#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect* BasicFX;
};
#pragma endregion

#endif // EFFECTS_H