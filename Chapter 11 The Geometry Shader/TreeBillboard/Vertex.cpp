#include "Vertex.h"
#include "Effects.h"

#pragma region InputLayoutDesc

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Basic32[3] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::TreePointSprite[2] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

#pragma endregion

#pragma region InputLayouts

ID3D11InputLayout* InputLayouts::Basic32         = 0;
ID3D11InputLayout* InputLayouts::TreePointSprite = 0;

void InputLayouts::InitAll(ID3D11Device* device)
{
	//
	// Basic32
	//
	HR(device->CreateInputLayout(InputLayoutDesc::Basic32, 3, Effects::BasicFX->getVSBlob()->GetBufferPointer(),
		Effects::BasicFX->getVSBlob()->GetBufferSize(), &Basic32));

	//
	// TreePointSprite
	//
	HR(device->CreateInputLayout(InputLayoutDesc::TreePointSprite, 2, Effects::TreeSpriteFX->getVSBlob()->GetBufferPointer(), 
		Effects::TreeSpriteFX->getVSBlob()->GetBufferSize(), &TreePointSprite));
}

void InputLayouts::DestroyAll()
{
	ReleaseCOM(Basic32);
	ReleaseCOM(TreePointSprite);
}

#pragma endregion
