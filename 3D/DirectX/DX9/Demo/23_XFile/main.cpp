#include "sims.h"
#include "core/log.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "utils/demo_app.h"
using namespace sims;

class XFile : public DemoApp<d3d9::Window>
{
public:
	XFile()
		: mesh_(nullptr)
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
			//	matD3D.Ambient = matD3D.Diffuse;

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

		// optimize mesh
		d3d9::CHECK_HR = mesh_->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
			(DWORD*)adjBuffer->GetBufferPointer(),
			0,
			0,
			0);
		SAFE_RELEASE(adjBuffer);

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
		D3DXVECTOR3 pos(4.0f, 4.0f, -13.0f);
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
			static float y = 0.0f;
			
			D3DXMATRIX ry;
			D3DXMatrixRotationY(&ry, y);
			y += timestep.GetSeconds();
			if (y >= 2 * D3DX_PI)
				y = 0.0f;
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &ry);

			// draw
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			for (int i = 0; i < mtrls_.size(); ++i)
			{
				auto& mtrl = mtrls_[i];
				d3d9::CHECK_HR = d3d9::g_pD3DD->SetMaterial(&mtrl);
				d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, texs_[i]);
				d3d9::CHECK_HR = mesh_->DrawSubset(i);
			}

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(mesh_);
		for (auto& tex : texs_)
		{
			SAFE_RELEASE(tex);
		}
	}


private:
	ID3DXMesh* mesh_;
	vector<D3DMATERIAL9> mtrls_;
	vector<IDirect3DTexture9*> texs_;
};

int main()
{
	XFile app;
	app.Create(640, 480, "X File");
	app.Loop();
	return 0;
}