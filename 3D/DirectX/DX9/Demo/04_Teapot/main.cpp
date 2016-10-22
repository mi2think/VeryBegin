#include "sims.h"
#include "core/log.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "utils/demo_app.h"
using namespace sims;

class Teapot : public DemoApp<d3d9::Window>
{
public:
	Teapot() : teapot_(nullptr) {}

	virtual void OnCreate()
	{
		DemoApp<d3d9::Window>::OnCreate();

		// create teapot 
		d3d9::CHECK_HR = D3DXCreateTeapot(d3d9::g_pD3DD, &teapot_, 0);

		// view
		D3DXVECTOR3 position(0.0f, 0.0f, -3.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &position, &target, &up);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.5f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);

		// wireframe mode
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		// spin teapot
		D3DXMATRIX ry;
		static float y = 0.0f;
		D3DXMatrixRotationY(&ry, y);
		y += timestep.GetSeconds();
		if (y >= 2 * M_PI)
			y = 0.0f;
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &ry);

		// draw teapot
		d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();
		teapot_->DrawSubset(0);
		d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
		d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(teapot_);
	}
private:
	ID3DXMesh* teapot_;
};

int main()
{
	Teapot app;
	app.Create(640, 480, "Teapot");
	app.Loop();
	return 0;
}