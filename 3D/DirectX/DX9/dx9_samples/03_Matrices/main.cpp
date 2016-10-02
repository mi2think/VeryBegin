#include "sims.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class Matrices : public DemoApp<dx9::Window>
{
public:
	struct CUSTOMVERTEX
	{
		float x, y, z;
		DWORD color;
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	};

	virtual void OnCreate()
	{
		// create vertex buffer
		CUSTOMVERTEX vertices[] =
		{
			{ -1.0f, -1.0f, 0.0f, 0xffff0000 },
			{  1.0f, -1.0f, 0.0f, 0xff0000ff },
			{  0.0f,  1.0f, 0.0f, 0xffffffff },
		};

		dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexBuffer(sizeof(vertices), 
			D3DUSAGE_WRITEONLY,
			CUSTOMVERTEX::FVF,
			D3DPOOL_DEFAULT,
			&vb_,
			0);
		CUSTOMVERTEX* v = nullptr;
		dx9::CHECK_HR = vb_->Lock(0, 0, (void**)&v, 0);
		memcpy(v, vertices, sizeof(vertices));
		dx9::CHECK_HR = vb_->Unlock();

		// setup matrices
		
		// view
		D3DXVECTOR3 pos(0.0f, 3.0f, -5.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &pos, &target, &up);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI / 4.0f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);

		// states
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, false);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			static float y = 0.0f;

			y += timestep.GetSeconds();
			if (y >= 2 * D3DX_PI)
				y = 0.0f;

			D3DXMATRIX world;
			D3DXMatrixRotationY(&world, y);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &world);

			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff0000ff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, vb_, 0, sizeof(CUSTOMVERTEX));
			dx9::CHECK_HR = dx9::g_pD3DD->SetFVF(CUSTOMVERTEX::FVF);
			dx9::CHECK_HR = dx9::g_pD3DD->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
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
	Matrices app;
	app.Create(300, 300, "Matrices");
	app.Loop();
	return 0;
}