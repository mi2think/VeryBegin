sampler baseTex;
sampler spotLightTex;
sampler textTex;

struct PS_INPUT
{
	float2 base      : TEXCOORD0;
	float2 spotLight : TEXCOORD1;
	float2 text      : TEXCOORD2;
};

struct PS_OUTPUT
{
	vector color : COLOR0;
};

PS_OUTPUT main(PS_INPUT i)
{
	PS_OUTPUT o = (PS_OUTPUT)0;

	vector b = tex2D(baseTex, i.base);
	vector s = tex2D(spotLightTex, i.spotLight);
	vector t = tex2D(textTex, i.text);

	vector c = b * s + t;
	c += 0.1f; // increase the intensity of the pixel slightly

	o.color = c;
	return o;
}