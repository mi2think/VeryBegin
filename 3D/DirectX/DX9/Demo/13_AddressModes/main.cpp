#include "sims.h"
#include "core/key_event.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_dx9.h"
using namespace sims;

class AddressModes : public DemoApp<dx9::Window>
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

	AddressModes()
		: vb_(nullptr)
		, ib_(nullptr)
		, tex_(nullptr)
	{}

	virtual void OnCreate()
	{
		DemoApp<dx9::Window>::OnCreate();

		// create buffer
		dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexBuffer(4 * sizeof(Vertex),
			D3DUSAGE_WRITEONLY,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&vb_,
			0);
		dx9::CHECK_HR = dx9::g_pD3DD->CreateIndexBuffer(6 * sizeof(WORD),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&ib_,
			0);

		// fill buffer
		Vertex* v = nullptr;
		dx9::CHECK_HR = vb_->Lock(0, 0, (void**)&v, 0);
		v[0] = Vertex(Vector3f(-1.0f, -1.0f, 1.25f), Vector2f(0.0f, 3.0f));
		v[1] = Vertex(Vector3f(-1.0f,  1.0f, 1.25f), Vector2f(0.0f, 0.0f));
		v[2] = Vertex(Vector3f( 1.0f,  1.0f, 1.25f), Vector2f(3.0f, 0.0f));
		v[3] = Vertex(Vector3f( 1.0f, -1.0f, 1.25f), Vector2f(3.0f, 3.0f));
		dx9::CHECK_HR = vb_->Unlock();

		WORD* i = nullptr;
		dx9::CHECK_HR = ib_->Lock(0, 0, (void**)&i, 0);
		WORD n[] = {
			0, 1, 2,
			0, 2, 3
		};
		memcpy(i, (void*)&n[0], sizeof(n));
		dx9::CHECK_HR = ib_->Unlock();

		// create texture and set texture filters
		dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD, "dx5_logo.bmp", &tex_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, tex_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// close lighting
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, false);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.5f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnRender(const Timestep&)
	{
		if (dx9::g_pD3DD)
		{
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, vb_, 0, sizeof(Vertex));
			dx9::CHECK_HR = dx9::g_pD3DD->SetIndices(ib_);
			dx9::CHECK_HR = dx9::g_pD3DD->SetFVF(Vertex::FVF);
			dx9::CHECK_HR = dx9::g_pD3DD->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual bool OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &AddressModes::OnKeyEvent);
		return dispatch.GetResult();
	}

	bool OnKeyEvent(const KeyPressEvent& event)
	{
		int key = event.GetKey();
		switch (key)
		{
		case KEY_KEY_1: // wrap
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			break;
		case KEY_KEY_2: // border
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x000000ff);
			break;
		case KEY_KEY_3: // clamp
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			break;
		case KEY_KEY_4: // mirror
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
			break;
		default:
			break;
		}
		return true;
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(vb_);
		SAFE_RELEASE(ib_);
		SAFE_RELEASE(tex_);
	}

private:
	IDirect3DVertexBuffer9* vb_;
	IDirect3DIndexBuffer9* ib_;
	IDirect3DTexture9* tex_;
};


int main()
{
	AddressModes app;
	app.Create(640, 480, "Address Modes");
	app.Loop();
	return 0;
}
