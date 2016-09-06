#include "sims.h"
#include "core/log.h"
#include "graphics_api/sims_dx9.h"
#include "utils/demo_app.h"
using namespace sims;

class ColorTriangle : public DemoApp<dx9::Window>
{
public:
	struct Vertex
	{
		float x;
		float y;
		float z;
		D3DCOLOR color;

		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

		Vertex(float _x, float _y, float _z, D3DCOLOR clr)
			: x(_x)
			, y(_y)
			, z(_z)
			, color(clr)
		{}
	};

	ColorTriangle() : vb_(nullptr) {}

	virtual void OnCreate()
	{
		DemoApp<dx9::Window>::OnCreate();

		// create vertex buffer
		dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexBuffer(3 * sizeof(Vertex),
			D3DUSAGE_WRITEONLY,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&vb_,
			0);

		// fill buffer
		Vertex* v = nullptr;
		dx9::CHECK_HR = vb_->Lock(0, 0, (void**)&v, 0);
		v[0] = Vertex(-1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
		v[1] = Vertex( 0.0f, 1.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
		v[2] = Vertex( 1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(0, 0, 255));
		vb_->Unlock();

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.5f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);

		// turn off lighting
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, false);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();
			dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, vb_, 0, sizeof(Vertex));
			dx9::CHECK_HR = dx9::g_pD3DD->SetFVF(Vertex::FVF);

			// draw left triangle with flat shading
			D3DXMatrixTranslation(&mt_, -1.25f, 0.0f, 0.0f);
			dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &mt_);
			dx9::CHECK_HR = dx9::g_pD3DD->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

			// draw right triangle with gouraud shading
			D3DXMatrixTranslation(&mt_, 1.25f, 0.0f, 0.0f);
			dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &mt_);
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
	D3DXMATRIX mt_;
};

int main()
{
	ColorTriangle app;
	app.Create(640, 480, "Color Triangle");
	app.Loop();
	return 0;
}