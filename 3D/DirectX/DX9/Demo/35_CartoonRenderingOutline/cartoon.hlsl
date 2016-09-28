matrix worldViewM;
matrix worldViewProjM;

vector color;
vector lightDir;

struct VS_OUTPUT
{
	vector position : POSITION;
	float2 uv       : TEXCOORD;
	vector diffuse  : COLOR;
};

VS_OUTPUT main(vector pos    : POSITION,
	           vector normal : NORMAL)
{
	VS_OUTPUT o = (VS_OUTPUT)0;

	o.position = mul(pos, worldViewProjM);

	vector L = mul(lightDir, worldViewM);
	vector N = normal;
	N.w = 0.0f;
	N = mul(N, worldViewM);

	float u = saturate(dot(L, N));

	o.uv.x = u;
	o.uv.y = 0.5f;

	o.diffuse = color;
	return o;
}