#include "sims.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class Meshes : public DemoApp<dx9::Window>
{
public:
	Meshes() 
		: xmesh_(nullptr)
	{
	}

	~Meshes()
	{
	}

	virtual void OnCreate()
	{
		xmesh_ = new dx9::XMesh("Tiger.x");

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
			100.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);

		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			static float y = 0.0f;

			y += timestep.GetSeconds();
			if (y >= 2 * D3DX_PI)
				y = 0.0f;

			// world
			D3DXMATRIX world;
			D3DXMatrixRotationY(&world, y);

			// draw
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff0000ff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			xmesh_->Render(&world);

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_DELETE(xmesh_);
	}

private:
	dx9::XMesh* xmesh_;
};

int main()
{
	Meshes app;
	app.Create(300, 300, "Meshes");
	app.Loop();
	return 0;
}