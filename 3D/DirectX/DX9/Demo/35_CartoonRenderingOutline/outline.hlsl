matrix worldViewM;
matrix projM;

static vector black = { 0.0f, 0.0f , 0.0f, 0.0f };

struct VS_INPUT
{
	vector position    : POSITION;
	vector normal      : NORMAL0;
	vector faceNormal1 : NORMAL1;
	vector faceNormal2 : NORMAL2;
};

struct VS_OUTPUT
{
	vector position    : POSITION;
	vector diffuse     : COLOR;
};

VS_OUTPUT main(VS_INPUT i)
{
	VS_OUTPUT o = (VS_OUTPUT)0;

	// transform position to view space
	i.position = mul(i.position, worldViewM);

	// compute a vector in direction of the vertex from the eye
	// recall the eye is just origin at view space
	vector eyeToVertex = i.position;

	// transform normal to view space
	i.normal.w = 0.0f;
	i.faceNormal1.w = 0.0f;
	i.faceNormal2.w = 0.0f;

	i.normal = mul(i.normal, worldViewM);
	i.faceNormal1 = mul(i.faceNormal1, worldViewM);
	i.faceNormal2 = mul(i.faceNormal2, worldViewM);

	// compute cosine of the angles between the eyeToVertex vector and the face normal
	float k0 = dot(eyeToVertex, i.faceNormal1);
	float k1 = dot(eyeToVertex, i.faceNormal2);

	if ((k0 * k1) < 0.0f)
		i.position += 0.1f * i.normal;

	o.position = mul(i.position, projM);
	o.diffuse = black;

	return o;
}