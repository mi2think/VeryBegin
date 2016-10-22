#include "sims.h"
#include "core/log.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "utils/demo_app.h"
#include <fstream>
using namespace sims;

void DumpVertices(std::ofstream& ofs, ID3DXMesh* mesh)
{
	ofs << "Vertices:\n";
	ofs << "---------------\n";
	
	d3d9::VertexUVN* v = nullptr;
	d3d9::CHECK_HR = mesh->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < mesh->GetNumVertices(); ++i)
	{
		ofs << "Vertex " << i << ": (";
		ofs << v[i].p.x << ", " << v[i].p.y << ", " << v[i].p.z << ", ";
		ofs << v[i].n.x << ", " << v[i].n.y << ", " << v[i].n.z << ", ";
		ofs << v[i].uv.x << ", " << v[i].uv.y << ")\n";
	}
	d3d9::CHECK_HR = mesh->UnlockVertexBuffer();

	ofs << "\n\n";
}

void DumpIndices(std::ofstream& ofs, ID3DXMesh* mesh)
{
	ofs << "Indices:\n";
	ofs << "---------------\n";

	WORD* w = nullptr;
	d3d9::CHECK_HR = mesh->LockIndexBuffer(0, (void**)&w);
	for (int i = 0; i < mesh->GetNumFaces(); ++i)
	{
		ofs << "Triangle " << i << ": ";
		ofs << w[i * 3] << " " << w[i * 3 + 1] << " " << w[i * 3 + 2] << "\n";
	}
	d3d9::CHECK_HR = mesh->UnlockIndexBuffer();

	ofs << "\n\n";
}

void DumpAttributeBuffer(std::ofstream& ofs, ID3DXMesh* mesh)
{
	ofs << "Attribute Buffer:\n";
	ofs << "---------------\n";

	DWORD* attrBuffer = nullptr;
	d3d9::CHECK_HR = mesh->LockAttributeBuffer(0, &attrBuffer);
	for (int i = 0; i < mesh->GetNumFaces(); ++i)
	{
		ofs << "Triangle lives in subset " << i << ": ";
		ofs << attrBuffer[i] << "\n";
	}
	d3d9::CHECK_HR = mesh->UnlockAttributeBuffer();

	ofs << "\n\n";
}

void DumpAdjacencyBuffer(std::ofstream& ofs, ID3DXMesh* mesh)
{
	ofs << "Adjacency Buffer:\n";
	ofs << "---------------\n";

	vector<DWORD> adjacencyBuffer(mesh->GetNumFaces() * 3);
	mesh->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);
	for (int i = 0; i < mesh->GetNumFaces(); ++i)
	{
		ofs << "Triangle's adjacent to triangle " << i << ": ";
		ofs << adjacencyBuffer[i * 3] << " " << adjacencyBuffer[i * 3 + 1] << " " << adjacencyBuffer[i * 3 + 2] << "\n";
	}

	ofs << "\n\n";
}

void DumpAttributeTable(std::ofstream& ofs, ID3DXMesh* mesh)
{
	ofs << "Attribute Table:\n";
	ofs << "---------------\n";

	// number of entries in the attribute table
	DWORD numEntries = 0;
	mesh->GetAttributeTable(0, &numEntries);
	vector<D3DXATTRIBUTERANGE> table(numEntries);
	mesh->GetAttributeTable(&table[0], &numEntries);
	for (int i = 0; i < numEntries; ++i)
	{
		ofs << "Entry " << i << "\n";
		ofs << "---------\n";

		ofs << "Subset ID:      " << table[i].AttribId << "\n";
		ofs << "Face Start:     " << table[i].FaceStart << "\n";
		ofs << "Face Count:     " << table[i].FaceCount << "\n";
		ofs << "Vertex Start:   " << table[i].VertexStart << "\n";
		ofs << "Vertex Count:   " << table[i].VertexCount << "\n";
	}

	ofs << "\n\n";
}



class D3DXCreateMeshFVFApp : public DemoApp<d3d9::Window>
{
public:
	D3DXCreateMeshFVFApp()
		: mesh_(nullptr)
		, tex0_(nullptr)
		, tex1_(nullptr)
		, tex2_(nullptr)
	{}


	virtual void OnCreate()
	{
		DemoApp<d3d9::Window>::OnCreate();

		// create empty mesh
		d3d9::CHECK_HR = D3DXCreateMeshFVF(
			12,
			24,
			D3DXMESH_MANAGED,
			d3d9::VertexUVN::FVF,
			d3d9::g_pD3DD,
			&mesh_);

		d3d9::VertexUVN n[24];
		WORD i[36];
		GeometryGen::GenBox(2.0f, 2.0f, 2.0f, 
			GeometryGen::VBDesc((uint8*)&n[0], sizeof(d3d9::VertexUVN), 0, d3d9::GeoBase::OFFSET_UV, d3d9::GeoBase::OFFSET_N),
			GeometryGen::IBDesc((uint8*)&i[0], GeometryGen::IBDesc::Index16), GeometryGen::VT_Position | GeometryGen::VT_Normal | GeometryGen::VT_UV);

		// fill vertices and indices
		d3d9::VertexUVN* v = nullptr;
		d3d9::CHECK_HR = mesh_->LockVertexBuffer(0, (void**)&v);
		memcpy(v, n, sizeof(n));
		d3d9::CHECK_HR = mesh_->UnlockVertexBuffer();

		WORD* w = nullptr;
		d3d9::CHECK_HR = mesh_->LockIndexBuffer(0, (void**)&w);
		memcpy(w, i, sizeof(i));
		d3d9::CHECK_HR = mesh_->UnlockIndexBuffer();

		// specify the subset each triangle belongs to.
		DWORD* attrBuffer = nullptr;
		d3d9::CHECK_HR = mesh_->LockAttributeBuffer(0, &attrBuffer);
		for (int i = 0; i < 12; ++i)
		{
			if (i < 4)
				attrBuffer[i] = 0;
			else if (i < 8)
				attrBuffer[i] = 1;
			else
				attrBuffer[i] = 2;
		}
		d3d9::CHECK_HR = mesh_->UnlockAttributeBuffer();

		// optimize the mesh to generate attribute table
		vector<DWORD> adjacencyBuffer(mesh_->GetNumFaces() * 3);
		mesh_->GenerateAdjacency(0.0f, &adjacencyBuffer[0]);
		d3d9::CHECK_HR = mesh_->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
			&adjacencyBuffer[0],
			0,
			0,
			0);
		
		// dump mesh data to file
		std::ofstream ofs("MeshDump.txt");
		DumpVertices(ofs, mesh_);
		DumpIndices(ofs, mesh_);
		DumpAttributeTable(ofs, mesh_);
		DumpAttributeBuffer(ofs, mesh_);
		DumpAdjacencyBuffer(ofs, mesh_);
		ofs.close();

		// load textures
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD,
			"brick0.jpg",
			&tex0_);
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD,
			"brick1.jpg",
			&tex1_);
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD,
			"checker.jpg",
			&tex2_);

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// disable lighting
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, false);

		// view
		D3DXVECTOR3 pos(0.0f, 0.0f, -4.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &pos, &target, &up);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.5f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (d3d9::g_pD3DD)
		{
			// spin cube
			D3DXMATRIX rx;
			D3DXMatrixRotationX(&rx, D3DX_PI * 0.2f);

			static float y = 0.0f;
			D3DXMATRIX ry;
			D3DXMatrixRotationY(&ry, y);
			y += timestep.GetSeconds();
			if (y > 2.0f * D3DX_PI)
				y = 0.0f;
			D3DXMATRIX r = rx * ry;
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &r);

			// draw
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			IDirect3DTexture9* texs[] = { tex0_, tex1_, tex2_ };
			for (int i = 0; i < 3; ++i)
			{
				d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, texs[i]);
				mesh_->DrawSubset(i);
			}

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(mesh_);
		SAFE_RELEASE(tex0_);
		SAFE_RELEASE(tex1_);
		SAFE_RELEASE(tex2_);
	}
private:
	ID3DXMesh* mesh_;
	IDirect3DTexture9* tex0_;
	IDirect3DTexture9* tex1_;
	IDirect3DTexture9* tex2_;
};

int main()
{
	D3DXCreateMeshFVFApp app;
	app.Create(640, 480, "D3DXCreateMeshFVF");
	app.Loop();
	return 0;
}