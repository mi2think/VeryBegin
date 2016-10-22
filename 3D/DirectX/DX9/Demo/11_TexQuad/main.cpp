#include "sims.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "utils/demo_app.h"
using namespace sims;

class TexQuad : public DemoApp<d3d9::Window>
{
public:
	struct Vertex
	{
		Vector3f p;
		Vector2f uv;

		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;

		Vertex(const Vector3f& _p, const Vector2f& _uv)
			: p(_p)
			, uv(_uv)
		{}
	};


	TexQuad() : vb_(nullptr), tex_(nullptr) {}

	virtual void OnCreate()
	{
		DemoApp<d3d9::Window>::OnCreate();

		// create vertex buffer
		d3d9::CHECK_HR = d3d9::g_pD3DD->CreateVertexBuffer(6 * sizeof(Vertex),
			D3DUSAGE_WRITEONLY,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&vb_,
			0);
		
		// fill vertex buffer
		Vertex* v = nullptr;
		d3d9::CHECK_HR = vb_->Lock(0, 0, (void**)&v, 0);
		v[0] = Vertex(Vector3f(-1.0f, -1.0f, 1.25f), Vector2f(0.0f, 1.0f));
		v[1] = Vertex(Vector3f(-1.0f,  1.0f, 1.25f), Vector2f(0.0f, 0.0f));
		v[2] = Vertex(Vector3f( 1.0f,  1.0f, 1.25f), Vector2f(1.0f, 0.0f));

		v[3] = Vertex(Vector3f(-1.0f, -1.0f, 1.25f), Vector2f(0.0f, 1.0f));
		v[4] = Vertex(Vector3f( 1.0f,  1.0f, 1.25f), Vector2f(1.0f, 0.0f));
		v[5] = Vertex(Vector3f( 1.0f, -1.0f, 1.25f), Vector2f(1.0f, 1.0f));
		d3d9::CHECK_HR = vb_->Unlock();

		// do not use lighting
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, false);

		// create texture and set filters
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD, "dx5_logo.bmp", &tex_);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, tex_);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.5f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnRender(const Timestep& timpstep)
	{
		if (d3d9::g_pD3DD)
		{
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			d3d9::CHECK_HR = d3d9::g_pD3DD->SetStreamSource(0, vb_, 0, sizeof(Vertex));
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetFVF(Vertex::FVF);
			d3d9::CHECK_HR = d3d9::g_pD3DD->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(vb_);
		SAFE_RELEASE(tex_);
	}
private:
	IDirect3DVertexBuffer9* vb_;
	IDirect3DTexture9* tex_;
};

int main()
{
	TexQuad app;
	app.Create(640, 480, "Tex Quad");
	app.Loop();
	return 0;
}