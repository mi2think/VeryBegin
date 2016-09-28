#pragma once
#include "sims.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

struct EdgeVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 faceNormal1;
	D3DXVECTOR3 faceNormal2;
};

struct MeshVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	static const DWORD FVF;
};

class SilhouetteEdges
{
public:
	SilhouetteEdges(IDirect3DDevice9* device,
		ID3DXMesh* mesh,
		ID3DXBuffer* adjBuffer);
	~SilhouetteEdges();

	void Render();

private:
	void CreateVertexDecl();
	void GenEdgeVertices(ID3DXMesh* mesh, ID3DXBuffer* adjBuffer);
	void GenEdgeIndices(ID3DXMesh* mesh);
	void GetFaceNormal(DWORD faceIndex, 
		D3DXVECTOR3* faceNormal,
		MeshVertex* vertices,
		WORD* indices);
	void GetFaceNormals(ID3DXBuffer* adjBuffer, 
		D3DXVECTOR3* currFaceNormal, 
		D3DXVECTOR3 adjFaceNormal[3], 
		DWORD faceIndex,
		MeshVertex* vertices,
		WORD* indices);
private:
	IDirect3DDevice9* device_;
	IDirect3DVertexBuffer9* vb_;
	IDirect3DIndexBuffer9* ib_;
	IDirect3DVertexDeclaration9* decl_;

	uint32 numVertices_;
	uint32 numFaces_;
};