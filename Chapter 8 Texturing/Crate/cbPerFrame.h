#pragma once
#include <DirectXMath.h>
#include "LightHelper.h"
struct cbPerFrame
{
	DirectionalLight mDirLights[3];
	DirectX::XMFLOAT3 mEyePosW;
	float mFogStart;
	float mFogRange;
	DirectX::XMFLOAT4 mFogColor;
};