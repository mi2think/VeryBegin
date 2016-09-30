technique Fog
{
	pass P0
	{
		// set misc render states
		
		pixelshader = null;
		vertexshader = null;
		fvf = XYZ | Normal;
		Lighting = true;
		NormalizeNormals = true;
		SpecularEnable = false;

		// fog states

		FogVertexMode = LINEAR; // linear fog function
		FogStart = 50.0f; // fog starts 50 units away from view point
		FogEnd = 300.0f;  // fog ends 300 units away from view point

		FogColor = 0x00cccccc; // gray
		FogEnable = true;
	}
}