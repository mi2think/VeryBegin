#include "sims.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class Vertices : public DemoApp<dx9::Window>
{
public:
	struct CUSTOMVERTEX
	{
		float x, y, z, rhw;
		DWORD color;
		static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	};

	Vertices()
		: vb_(nullptr)
	{}

	virtual void OnCreate()
	{
		CUSTOMVERTEX vertices[] = 
		{
			{ 150.0f,  50.0f,  0.5f, 1.0f, 0xffff0000, }, // x, y, z, rhw, color
			{ 250.0f,  250.0f, 0.5f, 1.0f, 0xff00ff00, },
			{ 50.0f,   250.0f, 0.5f, 1.0f, 0xff00ffff, },
		};
		dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexBuffer(sizeof(vertices),
			D3DUSAGE_WRITEONLY,
			CUSTOMVERTEX::FVF,
			D3DPOOL_DEFAULT,
			&vb_,
			nullptr);
		Vertices* v = nullptr;
		dx9::CHECK_HR = vb_->Lock(0, sizeof(vertices), (void**)&v, 0);
		memcpy(v, vertices, sizeof(vertices));
		dx9::CHECK_HR = vb_->Unlock();
	}

	virtual void OnRender(const Timestep&)
	{
		if (dx9::g_pD3DD)
		{
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
	Vertices app;
	app.Create(300, 300, "Vertices");
	app.Loop();
	return 0;
}
