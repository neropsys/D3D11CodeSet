#pragma once
#include <DirectXMath.h>
#include "LightHelper.h"
struct cbPerObject
{
	DirectX::XMFLOAT4X4 mWorld;
	DirectX::XMFLOAT4X4 mWorldInvTranspose;
	DirectX::XMFLOAT4X4 mWorldViewProj;
	DirectX::XMFLOAT4X4 mTexTransform;
	Material mMaterial;
};