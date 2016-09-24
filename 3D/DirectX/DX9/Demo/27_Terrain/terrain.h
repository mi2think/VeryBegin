#pragma once

#include "sims.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class Terrain
{
public:
	Terrain(IDirect3DDevice9* device,
		const string& heightMapFilename,
		float heightScale,
		float width,
		float depth,
		uint32 slices,
		uint32 stacks);
	~Terrain();

	float GetHeightmapEntry(uint32 row, uint32 col) const;
	void SetHeightmapEntry(uint32 row, uint32 col, float value);
	float GetHeight(float x, float z);

	float ComputeStandAngle(float x, float z);

	bool LoadTexture(const string& filename);
	bool GenerateTexture(const Vector3f& dirLight);
	void Draw(const D3DXMATRIX& world, bool drawWireframe);
private:
	bool ReadRawFile(const string& heightMapFilename);
	void GenerateVertices();
	void GenerateIndices();
	bool LightTerrain(const Vector3f& dirLight);
	float ComputeShade(uint32 cellRow, uint32 cellCol, const Vector3f& dirLight);
private:
	float  width_; 
	float  depth_;
	uint32 slices_; // vertices in row(x-axis)
	uint32 stacks_; // vertices in col(z-axis)
	uint32 numVertices_;
	uint32 numIndices_;
	uint32 numTriangles_;

	// scale height from height map
	float  heightScale_;
	// at least have numVertices_ pixels
	vector<float> heightmap_;

	IDirect3DDevice9* device_;
	IDirect3DVertexBuffer9* vb_;
	IDirect3DIndexBuffer9* ib_;
	IDirect3DTexture9* tex_;

	dx9::VertexUV* vertices_;
	uint16* indices_;
};
