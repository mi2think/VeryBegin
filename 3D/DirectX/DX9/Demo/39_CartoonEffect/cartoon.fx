matrix worldViewM;
matrix worldViewProjM;

vector color;
vector lightDir;
texture tex;

struct VS_INPUT
{
	vector position : POSITION;
	vector normal   : NORMAL;
};

struct VS_OUTPUT
{
	vector position : POSITION;
	float2 uv       : TEXCOORD;
	vector color    : COLOR;
};

VS_OUTPUT main(VS_INPUT i)
{
	VS_OUTPUT o = (VS_OUTPUT)0;
	o.position = mul(i.position, worldViewProjM);

	i.normal.w = 0.0f;
	i.normal = mul(i.normal, worldViewM);

	vector lightD = mul(lightDir, worldViewM);

	float k = dot(lightD, i.normal);
	if (k < 0.0f)
		k = 0.0f;

	o.uv.x = k;
	o.uv.y = 0.5f;
	o.color = color;
	return o;
}

// sampler
sampler S0 = sampler_state
{
	Texture = (tex);
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;
};

// effect
technique Cartoon
{
	pass P0
	{
		vertexshader = compile vs_1_1 main();

		Sampler[0] = (S0);
	}
}