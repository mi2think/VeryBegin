#include "sims.h"
#include "core/log.h"
#include "math/vector3.h"
#include "graphics_api/sims_sdk_dx9.h"
#include "utils/demo_app.h"
using namespace sims;

class Triangle : public DemoApp<dx9::Window>
{
public:
	struct Vertex
	{
		Vector3f pos;
		static const DWORD FVF = D3DFVF_XYZ;

		Vertex(const Vector3f& p) : pos(p) {}
	};

	Triangle() : vb_(nullptr) {}

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
		v[0] = Vertex(Vector3f(-1.0f, 0.0f, 2.0f));
		v[1] = Vertex(Vector3f( 0.0f, 1.0f, 2.0f));
		v[2] = Vertex(Vector3f( 1.0f, 0.0f, 2.0f));
		dx9::CHECK_HR = vb_->Unlock();

		// set projection matrix
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.5f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);

		// set wireframe mode render state
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3D)
		{
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, vb_, 0, sizeof(Vertex));
			dx9::CHECK_HR = dx9::g_pD3DD->SetFVF(Vertex::FVF);
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
	Triangle app;
	app.Create(640, 480, "Triangle");
	app.Loop();
	return 0;
}