#include "silhouette_edges.h"

SilhouetteEdges::SilhouetteEdges(IDirect3DDevice9* device,
	ID3DXMesh* mesh,
	ID3DXBuffer* adjBuffer)
	: device_(device)
	, vb_(nullptr)
	, ib_(nullptr)
	, decl_(nullptr)
	, numVertices_(0)
	, numFaces_(0)
{
	GenEdgeVertices(mesh, adjBuffer);
	GenEdgeIndices(mesh);
	CreateVertexDecl();
}

SilhouetteEdges::~SilhouetteEdges()
{
	SAFE_RELEASE(vb_);
	SAFE_RELEASE(ib_);
	SAFE_RELEASE(decl_);
}

void SilhouetteEdges::GenEdgeVertices(ID3DXMesh* mesh, ID3DXBuffer* adjBuffer)
{
	// 3 edges per face
	// 4 vertices per edge
	numVertices_ = mesh->GetNumFaces() * 3 * 4;
	d3d9::CHECK_HR = device_->CreateVertexBuffer(numVertices_ * sizeof(EdgeVertex),
		D3DUSAGE_WRITEONLY,
		0, // using vertex declaration
		D3DPOOL_MANAGED,
		&vb_,
		0);

	MeshVertex* vertices = nullptr;
	d3d9::CHECK_HR = mesh->LockVertexBuffer(0, (void**)&vertices);

	WORD* indices = nullptr;
	d3d9::CHECK_HR = mesh->LockIndexBuffer(0, (void**)&indices);

	EdgeVertex* edgeV = nullptr;
	vb_->Lock(0, 0, (void**)&edgeV, 0);
	
	for (int i = 0; i < mesh->GetNumFaces(); ++i)
	{
		D3DXVECTOR3 currFaceNormal;
		D3DXVECTOR3 adjFaceNormals[3];

		GetFaceNormals(adjBuffer, &currFaceNormal, adjFaceNormals, i, vertices, indices);

		// get indices for this face
		WORD index0 = indices[i * 3];
		WORD index1 = indices[i * 3 + 1];
		WORD index2 = indices[i * 3 + 2];

		// get vertices for this face
		MeshVertex v0 = vertices[index0];
		MeshVertex v1 = vertices[index1];
		MeshVertex v2 = vertices[index2];

		// A        B
		// *--------*  
		// |  edge  |
		// *--------*
		// C        D
		// note, C and D are duplicates of A and B respectively,
		// such that the quad is degenerate. the vertex shader will
		// un-degenerate the quad if it is a silhouette edge

		auto GenQuad = [&currFaceNormal](EdgeVertex quad[],
			const MeshVertex& v0,
			const MeshVertex& v1,
			const D3DXVECTOR3& adjFaceNormal)
		{
			quad[0].position = v0.position;
			quad[0].normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			quad[0].faceNormal1 = currFaceNormal;
			quad[0].faceNormal2 = adjFaceNormal;

			quad[1].position = v1.position;
			quad[1].normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			quad[1].faceNormal1 = currFaceNormal;
			quad[1].faceNormal2 = adjFaceNormal;

			quad[2] = quad[0];
			quad[2].normal = v0.normal;
			
			quad[3] = quad[1];
			quad[3].normal = v1.normal;
		};

		EdgeVertex edgeVertices[4];

		// compute edge0 v0->v1, node adjacent face normal is faceNormal0
		GenQuad(edgeVertices, v0, v1, adjFaceNormals[0]);
		memcpy(edgeV, edgeVertices, sizeof(edgeVertices));
		edgeV += 4;

		// compute edge1 v1->v2, node adjacent face normal is faceNormal1
		GenQuad(edgeVertices, v1, v2, adjFaceNormals[1]);
		memcpy(edgeV, edgeVertices, sizeof(edgeVertices));
		edgeV += 4;

		// compute edge2 v0->v2, node adjacent face normal is faceNormal1
		GenQuad(edgeVertices, v0, v2, adjFaceNormals[2]);
		memcpy(edgeV, edgeVertices, sizeof(edgeVertices));
		edgeV += 4;
	}

	d3d9::CHECK_HR = vb_->Unlock();
	d3d9::CHECK_HR = mesh->UnlockVertexBuffer();
	d3d9::CHECK_HR = mesh->UnlockIndexBuffer();
}

void SilhouetteEdges::GetFaceNormal(DWORD faceIndex,
	D3DXVECTOR3* faceNormal,
	MeshVertex* vertices,
	WORD* indices)
{
	// get the triangle's indices
	WORD index0 = indices[faceIndex * 3];
	WORD index1 = indices[faceIndex * 3 + 1];
	WORD index2 = indices[faceIndex * 3 + 2];

	// now extract the triangles vertices position
	D3DXVECTOR3 v0 = vertices[index0].position;
	D3DXVECTOR3 v1 = vertices[index1].position;
	D3DXVECTOR3 v2 = vertices[index2].position;

	// compute face normal
	D3DXVECTOR3 edge0 = v1 - v0;
	D3DXVECTOR3 edge1 = v2 - v0;
	D3DXVec3Cross(faceNormal, &edge0, &edge1);
	D3DXVec3Normalize(faceNormal, faceNormal);
}

void SilhouetteEdges::GetFaceNormals(ID3DXBuffer* adjBuffer, 
	D3DXVECTOR3* currFaceNormal, 
	D3DXVECTOR3 adjFaceNormal[3], 
	DWORD faceIndex,
	MeshVertex* vertices,
	WORD* indices)
{
	DWORD* adj = (DWORD*)adjBuffer->GetBufferPointer();

	// get face normal
	GetFaceNormal(faceIndex, currFaceNormal, vertices, indices);

	// get adjacent face indices
	WORD faceIndex0 = adj[faceIndex * 3];
	WORD faceIndex1 = adj[faceIndex * 3 + 1];
	WORD faceIndex2 = adj[faceIndex * 3 + 2];

	// get adjacent face normals, if there is no adjacent face,
	// then set the adjacent face normal to the opposite of the "currFaceNormal".
	// recall we do this because edges that don't have an adjacent triangle are automatically
	// considered silhouette edges. also recall that an entry in adjacency buffer equal to -1
	// denotes that the edge doesn't have an adjacent triangle

	D3DXVECTOR3 faceNormal0, faceNormal1, faceNormal2;

	if (faceIndex0 != WORD(-1))
		GetFaceNormal(faceIndex0, &faceNormal0, vertices, indices);
	else
		faceNormal0 = -(*currFaceNormal);

	if (faceIndex1 != WORD(-1))
		GetFaceNormal(faceIndex1, &faceNormal1, vertices, indices);
	else
		faceNormal1 = -(*currFaceNormal);

	if (faceIndex2 != WORD(-1))
		GetFaceNormal(faceIndex2, &faceNormal2, vertices, indices);
	else
		faceNormal2 = -(*currFaceNormal);

	// save adjacent face normals
	adjFaceNormal[0] = faceNormal0;
	adjFaceNormal[1] = faceNormal1;
	adjFaceNormal[2] = faceNormal2;
}

void SilhouetteEdges::GenEdgeIndices(ID3DXMesh* mesh)
{
	DWORD numEdges = mesh->GetNumFaces() * 3;
	numFaces_ = numEdges * 2;

	d3d9::CHECK_HR = device_->CreateIndexBuffer(numFaces_ * 3 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&ib_,
		0);
	
	WORD* indices = nullptr;
	d3d9::CHECK_HR = ib_->Lock(0, 0, (void**)&indices, 0);
	// 0       1
	// *-------*
	// | edge  |
	// *-------*
	// 2       3

	for (uint32 i = 0; i < numEdges; ++i)
	{
		// six indices to define quad(the triangles of the edge),
		// so every edge we skip six entries in the index buffer.
		// for vertices we define the edge, so  every edge we skip
		// four entries in the vertex buffer.

		indices[i * 6] = i * 4;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 1;
		indices[i * 6 + 4] = i * 4 + 3;
		indices[i * 6 + 5] = i * 4 + 2;
	}
	d3d9::CHECK_HR = ib_->Unlock();
}

void SilhouetteEdges::CreateVertexDecl()
{
	D3DVERTEXELEMENT9 decl[] =
	{
		// offset in bytes
		{ 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{ 0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0,  24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1 },
		{ 0,  36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 2 },
		D3DDECL_END()
	};

	d3d9::CHECK_HR = device_->CreateVertexDeclaration(decl, &decl_);
}

void SilhouetteEdges::Render()
{
	d3d9::CHECK_HR = device_->SetVertexDeclaration(decl_);
	d3d9::CHECK_HR = device_->SetStreamSource(0, vb_, 0, sizeof(EdgeVertex));
	d3d9::CHECK_HR = device_->SetIndices(ib_);
	d3d9::CHECK_HR = device_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0,
		0,
		numVertices_,
		0,
		numFaces_);
}