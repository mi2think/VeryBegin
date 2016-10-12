cbuffer MatrixBuffer
{
	matrix worldViewProjM;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : Color;
};

VOut main( float3 pos   : POSITION,
	       float4 color : COLOR)
{
	VOut o;
	o.position = mul(float4(pos, 1.0f), worldViewProjM);
	o.color = color;
	return pos;
}