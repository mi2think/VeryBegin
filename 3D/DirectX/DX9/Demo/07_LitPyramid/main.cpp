#include "sims.h"
#include "core/log.h"
#include "math/vector3.h"
#include "graphics_api/sims_sdk_dx9.h"
#include "utils/demo_app.h"
using namespace sims;

class LitPyramid : public DemoApp<dx9::Window>
{
public:
	struct Vertex
	{
		Vector3f p;
		Vector3f n;

		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

		Vertex(const Vector3f& _p, const Vector3f& _n)
			: p(_p)
			, n(_n)
		{}
	};

	LitPyramid() : vb_(nullptr) {}

	virtual void OnCreate()
	{
		DemoApp<dx9::Window>::OnCreate();

		// create vertex buffer
		dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexBuffer(12 * sizeof(Vertex),
			D3DUSAGE_WRITEONLY,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&vb_,
			0);

		// fill buffer
		Vertex* v = 0;
		dx9::CHECK_HR = vb_->Lock(0, 0, (void**)&v, 0);
		// front
		v[0] = Vertex(Vector3f(-1.0f, 0.0f, -1.0f), Vector3f(0.0f, 0.707f, -0.707f));
		v[1] = Vertex(Vector3f( 0.0f, 1.0f,  0.0f), Vector3f(0.0f, 0.707f, -0.707f));
		v[2] = Vertex(Vector3f( 1.0f, 0.0f, -1.0f), Vector3f(0.0f, 0.707f, -0.707f));
		// left
		v[3] = Vertex(Vector3f(-1.0f, 0.0f,  1.0f), Vector3f(-0.707f, 0.707f, 0.0f));
		v[4] = Vertex(Vector3f( 0.0f, 1.0f,  0.0f), Vector3f(-0.707f, 0.707f, 0.0f));
		v[5] = Vertex(Vector3f(-1.0f, 0.0f, -1.0f), Vector3f(-0.707f, 0.707f, 0.0f));
		// right
		v[6] = Vertex(Vector3f( 1.0f, 0.0f, -1.0f), Vector3f( 0.707f, 0.707f, 0.0f));
		v[7] = Vertex(Vector3f( 0.0f, 1.0f,  0.0f), Vector3f( 0.707f, 0.707f, 0.0f));
		v[8] = Vertex(Vector3f( 1.0f, 0.0f,  1.0f), Vector3f( 0.707f, 0.707f, 0.0f));

		v[9]  = Vertex(Vector3f( 1.0f, 0.0f, 1.0f), Vector3f( 0.0f, 0.707f, 0.707f));
		v[10] = Vertex(Vector3f( 0.0f, 1.0f, 0.0f), Vector3f( 0.0f, 0.707f, 0.707f));
		v[11] = Vertex(Vector3f(-1.0f, 0.0f, 1.0f), Vector3f( 0.0f, 0.707f, 0.707f));
		dx9::CHECK_HR = vb_->Unlock();

		// create and set material
		D3DMATERIAL9 mt;
		mt.Ambient  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		mt.Diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		mt.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		mt.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mt.Power = 5.0f;
		dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&mt);

		// setup a directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Ambient  = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
		light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

		// set and enable light
		dx9::CHECK_HR = dx9::g_pD3DD->SetLight(0, &light);
		dx9::CHECK_HR = dx9::g_pD3DD->LightEnable(0, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);

		// turn on specular lighting
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SPECULARENABLE, true);

		// view
		D3DXVECTOR3 position(0.0f, 1.0f, -3.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &position, &target, &up);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.5f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			// rotate y-axis
			static float y = 0.0f;
			D3DXMATRIX ry;
			D3DXMatrixRotationY(&ry, y);
			y += timestep.GetSeconds();
			if (y >= 2 * D3DX_PI)
				y = 0.0f;
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &ry);

			// draw scene
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();
			dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, vb_, 0, sizeof(Vertex));
			dx9::CHECK_HR = dx9::g_pD3DD->SetFVF(Vertex::FVF);

			dx9::CHECK_HR = dx9::g_pD3DD->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);

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
	LitPyramid app;
	app.Create(640, 480, "Lit Pyramid");
	app.Loop();
	return 0;
}
