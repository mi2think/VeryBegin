#include "sims.h"
#include "math/vector3.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_d3d9.h"
using namespace sims;

class Lights : public DemoApp<d3d9::Window>
{
public:
	struct CUSTOMVERTEX
	{
		Vector3f position;
		Vector3f normal;
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
	};


	Lights()
		: vb_(nullptr)
	{}

	virtual void OnCreate()
	{
		// create vertex buffer
		d3d9::CHECK_HR = d3d9::g_pD3DD->CreateVertexBuffer(50 * 2 * sizeof(CUSTOMVERTEX),
			D3DUSAGE_WRITEONLY,
			CUSTOMVERTEX::FVF,
			D3DPOOL_DEFAULT,
			&vb_,
			0);

		// fill vertex buffer. we are algorithmically generating a cylinder here,
		// including the normals, which are used for lighting
		CUSTOMVERTEX* v = nullptr;
		d3d9::CHECK_HR = vb_->Lock(0, 0, (void**)&v, 0);
		for (int i = 0; i < 50; ++i)
		{
			float delta = (2 * D3DX_PI * i) / (50 - 1);
			v[2 * i].position = Vector3f(sinf(delta), -1.0f, cosf(delta));
			v[2 * i].normal   = Vector3f(sinf(delta),  0.0f, cosf(delta));

			v[2 * i + 1].position = Vector3f(sinf(delta), 1.0f, cosf(delta));
			v[2 * i + 1].normal   = Vector3f(sinf(delta), 0.0f, cosf(delta));
		}
		d3d9::CHECK_HR = vb_->Unlock();

		// create material
		D3DMATERIAL9 mtrl;
		GenMaterial(mtrl, yellow, yellow, black, black, 0.0f);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetMaterial(&mtrl);

		// view
		D3DXVECTOR3 pos(0.0f, 3.0f, -5.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &pos, &target, &up);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI / 4.0f,
			(float)width_ / height_,
			1.0f,
			100.0f);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ZENABLE, true);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (d3d9::g_pD3DD)
		{
			static float x = 0.0f;
			static float y = 0.0f;

			y += timestep.GetSeconds();
			if (y >= 2 * D3DX_PI)
				y = 0.0f;
			x += timestep.GetSeconds();
			if (x >= 2 * D3DX_PI)
				x = 0.0f;

			// create light
			D3DLIGHT9 light;
			memset(&light, 0, sizeof(light));
			light.Type = D3DLIGHT_DIRECTIONAL;
			light.Diffuse.r = 1.0f;
			light.Diffuse.g = 1.0f;
			light.Diffuse.b = 1.0f;
			D3DXVECTOR3 dir(cosf(y), 0.0f, sinf(y));
			D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &dir);
			light.Range = 1000.0f;
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetLight(0, &light);
			d3d9::CHECK_HR = d3d9::g_pD3DD->LightEnable(0, true);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_AMBIENT, 0x00202020);

			// world
			D3DXMATRIX world;
			D3DXMatrixRotationX(&world, x);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &world);

			// draw

			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff0000ff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			d3d9::CHECK_HR = d3d9::g_pD3DD->SetStreamSource(0, vb_, 0, sizeof(CUSTOMVERTEX));
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetFVF(CUSTOMVERTEX::FVF);
			d3d9::CHECK_HR = d3d9::g_pD3DD->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * (50 - 1));

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(vb_);
	}
private:
	IDirect3DVertexBuffer9* vb_;
};

int main()
{
	Lights app;
	app.Create(300, 300, "Lights");
	app.Loop();
	return 0;
}