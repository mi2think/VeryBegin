//matrix worldM;
matrix viewM;
matrix viewProjM;

vector ambientMtrl;
vector diffuseMtrl;
vector lightDirection;

vector diffuseLightIntensity = { 0.0f, 0.0f, 1.0f, 1.0f };
vector ambientLightIntensity = { 0.0f, 0.0f, 0.2f, 1.0f };

struct VS_OUTPUT
{
	vector position : POSITION;
	vector color    : COLOR;
};


VS_OUTPUT main(vector pos    : POSITION,
	           vector normal : NORMAL)
{
	VS_OUTPUT o;
	o.position = mul(pos, viewProjM);

	normal.w = 0.0f;
	normal = mul(normal, viewM);

	vector lightDir = mul(lightDirection, viewM);
	float k = dot(lightDir, normal);
	if (k < 0.0f)
		k = 0.0f;

	o.color = ambientMtrl * ambientLightIntensity + k * (diffuseMtrl * diffuseLightIntensity);
	return o;
}