#pragma once
#include <DirectXMath.h>
#include "LightHelper.h"
struct cbPerFrame
{
	DirectionalLight mDirLights[3];
	DirectX::XMFLOAT3 mEyePosW;
	float padding0;
	float mFogStart;
	DirectX::XMFLOAT3 padding1;
	float mFogRange;
	DirectX::XMFLOAT3 padding2;
	DirectX::XMFLOAT4 mFogColor;
	int gLightCount;
	DirectX::XMFLOAT3 padding3;
	//sizeof boolean variable in hlsl is 32bit int, whereas sizeof boolean in cpu-side is 1 byte
	//so I used int in c++ struct to match the size

	int gUseTexture;//1byte
	DirectX::XMFLOAT3 padding4;
	int gAlphaClip;//1byte
	DirectX::XMFLOAT3 padding5;
	int gFogEnabled;//1byte

};