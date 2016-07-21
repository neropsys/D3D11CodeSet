//***************************************************************************************
// d3dUtil.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "d3dUtil.h"
#include <fstream>
#include <memory>
#include <stdint.h>
#include <d3dcompiler.h>
using namespace std;

void ExtractFrustumPlanes(XMFLOAT4 planes[6], CXMMATRIX M)
{//
	// Left
	//
	planes[0].x = M.r[0].m128_f32[3] + M.r[0].m128_f32[0];
	planes[0].y = M.r[1].m128_f32[3] + M.r[1].m128_f32[0];
	planes[0].z = M.r[2].m128_f32[3] + M.r[2].m128_f32[0];
	planes[0].w = M.r[3].m128_f32[3] + M.r[3].m128_f32[0];

	//
	// Right
	//
	planes[1].x = M.r[0].m128_f32[3] - M.r[0].m128_f32[0];
	planes[1].y = M.r[1].m128_f32[3] - M.r[1].m128_f32[0];
	planes[1].z = M.r[2].m128_f32[3] - M.r[2].m128_f32[0];
	planes[1].w = M.r[3].m128_f32[3] - M.r[3].m128_f32[0];

	//
	// Bottom
	//
	planes[2].x = M.r[0].m128_f32[3] + M.r[0].m128_f32[1];
	planes[2].y = M.r[1].m128_f32[3] + M.r[1].m128_f32[1];
	planes[2].z = M.r[2].m128_f32[3] + M.r[2].m128_f32[1];
	planes[2].w = M.r[3].m128_f32[3] + M.r[3].m128_f32[1];

	//
	// Top
	//
	planes[3].x = M.r[0].m128_f32[3] - M.r[0].m128_f32[1];
	planes[3].y = M.r[1].m128_f32[3] - M.r[1].m128_f32[1];
	planes[3].z = M.r[2].m128_f32[3] - M.r[2].m128_f32[1];
	planes[3].w = M.r[3].m128_f32[3] - M.r[3].m128_f32[1];

	//
	// Near
	//
	planes[4].x = M.r[0].m128_f32[2];
	planes[4].y = M.r[1].m128_f32[2];
	planes[4].z = M.r[2].m128_f32[2];
	planes[4].w = M.r[3].m128_f32[2];

	//
	// Far
	//
	planes[5].x = M.r[0].m128_f32[3] - M.r[0].m128_f32[2];
	planes[5].y = M.r[1].m128_f32[3] - M.r[1].m128_f32[2];
	planes[5].z = M.r[2].m128_f32[3] - M.r[2].m128_f32[2];
	planes[5].w = M.r[3].m128_f32[3] - M.r[3].m128_f32[2];

	// Normalize the plane equations.
	for (int i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&planes[i]));
		XMStoreFloat4(&planes[i], v);
	}
}
HRESULT ShaderHelper::LoadCompiledShader(const char *filename, ID3DBlob **blob) {

	ifstream ifs(filename, ios::binary);
	if (ifs.bad() || ifs.fail())
	{
		string failmsg = "Failed to load shader from ";
		failmsg.append(filename);
		return S_FALSE;
	}

	ifs.seekg(0, ios::end);
	size_t size = (size_t)ifs.tellg();
	auto buf = unique_ptr<char>(new char[size]);

	ifs.seekg(0, ios::beg);
	ifs.read(buf.get(), size);
	ifs.close();

	if (FAILED(D3DCreateBlob(size, blob)))
		return S_FALSE;

	memcpy((*blob)->GetBufferPointer(), buf.get(), size);
	return S_OK;

}