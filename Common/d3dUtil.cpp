//***************************************************************************************
// d3dUtil.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "d3dUtil.h"
#include <fstream>
#include <memory>
#include <stdint.h>
#include <d3dcompiler.h>
#include <vector>
#include "DirectXTK/DDSTextureLoader.h"
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

ID3D11ShaderResourceView* d3dHelper::CreateTexture2DArraySRV(ID3D11Device* device, ID3D11DeviceContext* context, std::vector<std::wstring>& filenames)
{
	//
	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to 
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	//
	UINT size = filenames.size();
	vector<ID3D11Texture2D*> srcTex(size);
	for (UINT i = 0; i < size; i++) {
		HR(CreateDDSTextureFromFileEx(
			device,
			context,
			filenames[i].c_str(),
			0,
			D3D11_USAGE_STAGING,
			0,
			D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
			0,
			false,
			reinterpret_cast<ID3D11Resource**>(&srcTex[i]),
			0));
	}
	//
	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	//
	D3D11_TEXTURE2D_DESC texElementDesc;
	ZeroMemory(&texElementDesc, sizeof(texElementDesc));

	srcTex[0]->GetDesc(&texElementDesc);


	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	HR(device->CreateTexture2D(&texArrayDesc, 0, &texArray));

	//
	// Copy individual texture elements into texture array.
	//

	// for each texture element...
	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		// for each mipmap level...
		for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{

			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			HR(context->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

			context->UpdateSubresource(
				texArray,
				D3D11CalcSubresource(
					mipLevel,
					texElement,
					texElementDesc.MipLevels),
				0,
				mappedTex2D.pData,
				mappedTex2D.RowPitch,
				mappedTex2D.DepthPitch);
			context->Unmap(srcTex[texElement], mipLevel);

		}
	}
	//
	// Create a resource view to the texture array.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	ID3D11ShaderResourceView* texArraySRV;
	HR(device->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV));

	//
	// Cleanup--we only need the resource view.
	//

	ReleaseCOM(texArray);

	for (UINT i = 0; i < size; ++i)
		ReleaseCOM(srcTex[i]);

	return texArraySRV;
}
