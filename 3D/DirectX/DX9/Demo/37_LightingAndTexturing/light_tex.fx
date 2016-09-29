matrix worldM;
matrix viewM;
matrix projM;

texture tex;

// sampler
sampler S0 = sampler_state
{
	Texture = (tex);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

// effect
technique lightAndTexture
{
	pass P0
	{
		// set misc render states
		pixelshader = null;
		vertexshader = null;
		fvf = XYZ | Normal | Tex1;
		Lighting = true;
		NormalizeNormals = true;
		SpecularEnable = false;

		// set transform states
		WorldTransform[0] = (worldM);
		ViewTransform = (viewM);
		ProjectionTransform = (projM);

		// set a light source at light index 0, we fill out all the
		// components for light[0] because  The Direct3D documentation recommends us
		// to fill out all components for best performance

		LightType[0] = Directional;
		LightAmbient[0] = { 0.2f, 0.2f, 0.2f, 1.0f };
		LightDiffuse[0] = { 1.0f, 1.0f, 1.0f, 1.0f };
		LightSpecular[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
		LightDirection[0] = { 1.0f, -1.0f, 1.0f, 0.0f };
		LightPosition[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
		LightFalloff[0] = 0.0f;
		LightRange[0] = 0.0f;
		LightTheta[0] = 0.0f;
		LightPhi[0] = 0.0f;
		LightAttenuation0[0] = 1.0f;
		LightAttenuation1[0] = 0.0f;
		LightAttenuation2[0] = 0.0f;

		// finally, enbale the light
		LightEnable[0] = true;

		// set material compoents, this is like calling
		// IDirect3DDevice9::SetMaterial.

		MaterialAmbient = { 1.0f, 1.0f, 1.0f, 1.0f };
		MaterialDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		MaterialEmissive = { 0.0f, 0.0f, 0.0f, 0.0f };
		MaterialPower = 1.0f;
		MaterialSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };

		// hook up the sampler object to sampler stage 0,
		// which is given by Sampler[0]
		
		Sampler[0] = (S0);
	}
}