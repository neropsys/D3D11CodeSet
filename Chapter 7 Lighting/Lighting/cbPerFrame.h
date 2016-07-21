#pragma once
#include <DirectXMath.h>
#include "LightHelper.h"
struct cbPerFrame {
	DirectionalLight mDirLight;
	PointLight mPointLight;
	SpotLight mSpotLight;
	DirectX::XMFLOAT3 mEyePosW;
};