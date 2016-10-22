#include "sims.h"
#include "core/log.h"
#include "core/key_event.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "utils/demo_app.h"
using namespace sims;

class ProgressiveMesh : public DemoApp<d3d9::Window>
{
public:
	ProgressiveMesh()
		: mesh_(nullptr)
		, pmesh_(nullptr)
	{}

	virtual void OnCreate()
	{
		DemoApp<d3d9::Window>::OnCreate();

		// load .x file
		ID3DXBuffer* adjBuffer = nullptr;
		ID3DXBuffer* mtrlBuffer = nullptr;
		DWORD numMtrls = 0;

		d3d9::CHECK_HR = D3DXLoadMeshFromX("bigship1.x",
			D3DXMESH_MANAGED,
			d3d9::g_pD3DD,
			&adjBuffer,
			&mtrlBuffer,
			0,
			&numMtrls,
			&mesh_);

		// extract the materials, and load textures
		if (mtrlBuffer != nullptr && numMtrls > 0)
		{
			D3DXMATERIAL* mt = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();
			for (int i = 0; i < numMtrls; ++i)
			{
				D3DMATERIAL9& matD3D = mt[i].MatD3D;

				// the MatD3D property does not have an ambient value set when its loaded, so set it now
				matD3D.Ambient = matD3D.Diffuse;

				// save the material
				mtrls_.push_back(matD3D);

				// check if the material has an associative texture
				if (mt[i].pTextureFilename != nullptr)
				{
					// load texture
					IDirect3DTexture9* tex = nullptr;
					d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD,
						mt[i].pTextureFilename,
						&tex);
					texs_.push_back(tex);
				}
				else
				{
					texs_.push_back(nullptr);
				}
			}
		}
		SAFE_RELEASE(mtrlBuffer);

		DWORD* adj = (DWORD*)adjBuffer->GetBufferPointer();

		// optimize mesh
		d3d9::CHECK_HR = mesh_->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT,
			adj,
			adj,  // must need!!!
			0,
			0);
		d3d9::CHECK_HR = D3DXValidMesh(mesh_, adj, nullptr);
		// generate progressive mesh
		d3d9::CHECK_HR = D3DXGeneratePMesh(mesh_,
			(DWORD*)adjBuffer->GetBufferPointer(),
			0,
			0,
			1,
			D3DXMESHSIMP_FACE,
			&pmesh_);

		SAFE_RELEASE(adjBuffer);

		// set origin max face
		pmesh_->SetNumFaces(pmesh_->GetMaxFaces());

		int a = pmesh_->GetMinFaces();
		int b = pmesh_->GetMaxFaces();

		// set texture filter
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// setup directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = white;
		light.Specular = white;
		light.Ambient = white;
		light.Direction = D3DXVECTOR3(1.0f, -1.0f, 1.0f);

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetLight(0, &light);
		d3d9::CHECK_HR = d3d9::g_pD3DD->LightEnable(0, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_SPECULARENABLE, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_NORMALIZENORMALS, true);

		// view
		D3DXVECTOR3 pos(8.0f, 4.0f, -12.0f);
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
			// draw
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			for (int i = 0; i < mtrls_.size(); ++i)
			{
				// draw pmesh
				auto& mtrl = mtrls_[i];
				d3d9::CHECK_HR = d3d9::g_pD3DD->SetMaterial(&mtrl);
				d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, texs_[i]);
				d3d9::CHECK_HR = pmesh_->DrawSubset(i);

				// draw wireframe outline
				D3DMATERIAL9 yellowMtrl;
				GenMaterial(yellowMtrl, yellow, yellow, yellow, black, 2.0f);
				d3d9::CHECK_HR = d3d9::g_pD3DD->SetMaterial(&yellowMtrl);
				d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
				d3d9::CHECK_HR = pmesh_->DrawSubset(i);
				d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			}

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual bool OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &ProgressiveMesh::OnKeyPressEvent);
		return dispatch.GetResult();
	}

	bool OnKeyPressEvent(const KeyPressEvent& event)
	{
		int key = event.GetKey();
		int numFaves = pmesh_->GetNumFaces();

		if (key == 'A')
		{
			// add a face, note SetNumFace will automatically clamp value

			// sometimes we must add more than one face to invert an edge collapse transformation
			pmesh_->SetNumFaces(numFaves + 10);
			if (pmesh_->GetNumFaces() == numFaves)
				pmesh_->SetNumFaces(numFaves + 12);
		}
		else if (key == 'S')
		{
			// remove a face
			pmesh_->SetNumFaces(numFaves - 10);
		}

		LOG_INFO("faces:%d", numFaves);
		return true;
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(mesh_);
		SAFE_RELEASE(pmesh_);
		for (auto& tex : texs_)
		{
			SAFE_RELEASE(tex);
		}
	}


private:
	ID3DXMesh* mesh_;
	ID3DXPMesh* pmesh_;
	vector<D3DMATERIAL9> mtrls_;
	vector<IDirect3DTexture9*> texs_;
};

int main()
{
	ProgressiveMesh app;
	app.Create(640, 480, "Progressive Mesh");
	app.Loop();
	return 0;
}