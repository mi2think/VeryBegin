#include "sims.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "core/key_event.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_d3d9.h"
using namespace sims;

#define red   D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)
#define green D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)
#define blue  D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)
#define white D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
#define black D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)
#define yellow D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f)

#define GenMaterial(m, a, d, s, e, p) { m.Ambient = a; m.Diffuse = d, m.Specular = s; m.Emissive = e; m.Power = p; } 

class MtrlAlpha : public DemoApp<d3d9::Window>
{
public:
	struct Vertex
	{
		Vector3f p;
		Vector3f n;
		Vector2f uv;

		Vertex(const Vector3f& _p, const Vector3f& _n, const Vector2f& _uv)
			: p(_p)
			, n(_n)
			, uv(_uv)
		{}

		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	};

	MtrlAlpha()
		: vb_(nullptr)
		, ib_(nullptr)
		, tex_(nullptr)
	{}

	virtual void OnCreate()
	{
		DemoApp<d3d9::Window>::OnCreate();

		// bk
		{
			// create buffer
			d3d9::CHECK_HR = d3d9::g_pD3DD->CreateVertexBuffer(4 * sizeof(Vertex),
				D3DUSAGE_WRITEONLY,
				Vertex::FVF,
				D3DPOOL_MANAGED,
				&vb_,
				0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->CreateIndexBuffer(6 * sizeof(WORD),
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&ib_,
				0);

			// fill buffer
			Vertex* v = nullptr;
			d3d9::CHECK_HR = vb_->Lock(0, 0, (void**)&v, 0);
			v[0] = Vertex(Vector3f(-10.0f, -10.0f, 5.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector2f(0.0f, 1.0f));
			v[1] = Vertex(Vector3f(-10.0f, 10.0f, 5.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector2f(0.0f, 0.0f));
			v[2] = Vertex(Vector3f(10.0f, 10.0f, 5.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector2f(1.0f, 0.0f));
			v[3] = Vertex(Vector3f(10.0f, -10.0f, 5.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector2f(1.0f, 1.0f));
			d3d9::CHECK_HR = vb_->Unlock();

			WORD* i = nullptr;
			d3d9::CHECK_HR = ib_->Lock(0, 0, (void**)&i, 0);
			WORD n[] = {
				0, 1, 2,
				0, 2, 3
			};
			memcpy(i, (void*)&n[0], sizeof(n));
			d3d9::CHECK_HR = ib_->Unlock();
		}

		// teapot
		d3d9::CHECK_HR = D3DXCreateTeapot(d3d9::g_pD3DD, &teapot_, 0);

		// setup directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
		light.Ambient = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
		light.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);

		// set and enable light
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetLight(0, &light);
		d3d9::CHECK_HR = d3d9::g_pD3DD->LightEnable(0, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_SPECULARENABLE, true);

		// create texture and set texture filter
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD, "crate.jpg", &tex_);

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// init material
		GenMaterial(bkMtrl_, white, white, white, black, 2.0f);
		GenMaterial(teapotMtrl_, red, red, red, black, 2.0f);
		teapotMtrl_.Diffuse.a = 0.5f; // set alpha to 50% opacity


		// set alpha blending state
		{
			// use alpha in material's diffuse component for alpha
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

			// set blending factors so that alpha component determines transparency
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); // default
		}

		// view
		D3DXVECTOR3 pos(0.0f, 0.0f, -3.0f);
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

	virtual void OnRender(const Timestep&)
	{
		if (d3d9::g_pD3DD)
		{
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			// draw bk
			D3DXMATRIX m;
			D3DXMatrixIdentity(&m);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetFVF(Vertex::FVF);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetStreamSource(0, vb_, 0, sizeof(Vertex));
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetMaterial(&bkMtrl_);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, tex_);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetIndices(ib_);
			d3d9::CHECK_HR = d3d9::g_pD3DD->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

			// draw teapot
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

			D3DXMatrixScaling(&m, 1.5f, 1.5f, 1.5f);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetMaterial(&teapotMtrl_);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, 0);
			d3d9::CHECK_HR = teapot_->DrawSubset(0);

			d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual bool OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &MtrlAlpha::OnKeyEvent);
		return dispatch.GetResult();
	}

	bool OnKeyEvent(const KeyPressEvent& event)
	{
		int key = event.GetKey();
		switch (key)
		{
		case KEY_UP:
			teapotMtrl_.Diffuse.a += 0.01f;
			break;
		case KEY_DOWN:
			teapotMtrl_.Diffuse.a -= 0.01f;
			break;
		default:
			break;
		}
		clamp_t(teapotMtrl_.Diffuse.a, 0.0f, 1.0f);
		return true;
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(vb_);
		SAFE_RELEASE(ib_);
		SAFE_RELEASE(tex_);
		SAFE_RELEASE(teapot_);
	}

private:
	IDirect3DVertexBuffer9* vb_;
	IDirect3DIndexBuffer9* ib_;
	IDirect3DTexture9* tex_;
	ID3DXMesh* teapot_;

	D3DMATERIAL9 bkMtrl_;
	D3DMATERIAL9 teapotMtrl_;
};

int main()
{
	MtrlAlpha app;
	app.Create(640, 480, "Mtrl Alpha");
	app.Loop();
	return 0;
}