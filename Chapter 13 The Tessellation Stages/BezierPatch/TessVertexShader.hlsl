
struct VertexIn
{
	float3 PosL    : POSITION;
};

struct VertexOut
{
	float3 PosL    : POSITION;
};


// In Tessellation pipeline, VS is mostly used for animation to animate a coarse mesh
// before tessellating it.
VertexOut main(VertexIn vin)
{
	VertexOut vout;

	vout.PosL = vin.PosL;

	return vout;
}
