struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};
struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
};
cbuffer cbPerObject: register(b0)
{
	float4x4 gViewProj;
	Material gMaterial;
};
cbuffer cbPerFrame: register(b1)
{
	DirectionalLight gDirLights[3];//16byte aligned
	float3 gEyePosW;
	float padding0;//16byte aligned
	float  gFogStart;
	float3 padding1;
	float  gFogRange;
	float3 padding2;
	float4 gFogColor;
	int gLightCount;//4byte
	float3 padding3;
	bool gUseTexture;//1byte
	float3 padding4;
	bool gAlphaClip;//1byte
	float3 padding5;
	bool gFogEnabled;//1byte

};
// Nonnumeric values cannot be added to a cbuffer.
Texture2DArray gTreeMapArray: register(t0);
SamplerState samLinear: register(s0);

struct GeoOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex     : TEXCOORD;
	uint   PrimID  : SV_PrimitiveID;
};

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeDirectionalLight(Material mat, DirectionalLight L,
	float3 normal, float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.Direction;

	// Add ambient term.
	ambient = mat.Ambient * L.Ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}
}


float4 main(GeoOut pin) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);

// The toEye vector is used in lighting.
float3 toEye = gEyePosW - pin.PosW;

// Cache the distance to the eye from this surface point.
float distToEye = length(toEye);

// Normalize.
toEye /= distToEye;

// Default to multiplicative identity.
float4 texColor = float4(1, 1, 1, 1);
if (gUseTexture)
{
	// Sample texture.
	float3 uvw = float3(pin.Tex, pin.PrimID % 4);
	texColor = gTreeMapArray.Sample(samLinear, uvw);

	if (gAlphaClip)
	{
		// Discard pixel if texture alpha < 0.05.  Note that we do this
		// test as soon as possible so that we can potentially exit the shader 
		// early, thereby skipping the rest of the shader code.
		clip(texColor.a - 0.05f);
	}
}

//
// Lighting.
//

float4 litColor = texColor;
if (gLightCount > 0)
{
	// Start with a sum of zero.
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.  
	[unroll]
	for (int i = 0; i < gLightCount; ++i)
	{
		float4 A, D, S;
		ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEye,
			A, D, S);

		ambient += A;
		diffuse += D;
		spec += S;
	}

	// Modulate with late add.
	litColor = texColor*(ambient + diffuse) + spec;
}

//
// Fogging
//

if (gFogEnabled)
{
	float fogLerp = saturate((distToEye - gFogStart) / gFogRange);

	// Blend the fog color and the lit color.
	litColor = lerp(litColor, gFogColor, fogLerp);
}

// Common to take alpha from diffuse material and texture.
litColor.a = gMaterial.Diffuse.a * texColor.a;

return litColor;
}
