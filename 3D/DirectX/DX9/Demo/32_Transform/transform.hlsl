matrix viewProjM;

struct VS_OUTPUT
{
	vector position : POSITION;
	vector color  : COLOR;
};

VS_OUTPUT main(float4 pos : POSITION)
{
	VS_OUTPUT o;
	o.position = mul(pos, viewProjM);
	o.color = vector(1.0f,0.0f,1.0f,1.0f);
	return o;
}

