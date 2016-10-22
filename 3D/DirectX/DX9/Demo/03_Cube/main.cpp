#include "sims.h"
#include "core/log.h"
#include "math/vector3.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "utils/demo_app.h"
using namespace sims;

class Cube : public DemoApp<d3d9::Window>
{
public:
	struct Vertex
	{
		Vector3f pos;
		static const DWORD FVF = D3DFVF_XYZ;

		Vertex(const Vector3f& p) : pos(p) {}
	};

	Cube()
		: vb_(nullptr)
		, ib_(nullptr)
	{}

	virtual void OnCreate()
	{
		DemoApp<d3d9::Window>::OnCreate();

		// create vertex buffer and index buffer
		d3d9::CHECK_HR = d3d9::g_pD3DD->CreateVertexBuffer(8 * sizeof(Vertex),
			D3DUSAGE_WRITEONLY,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&vb_,
			0);
		d3d9::CHECK_HR = d3d9::g_pD3DD->CreateIndexBuffer(6 * 2 * 3 * sizeof(WORD),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&ib_,
			0);

		// fill buffer
		Vertex* v = nullptr;
		d3d9::CHECK_HR = vb_->Lock(0, 0, (void**)&v, 0);
		v[0] = Vertex(Vector3f(-1.0f, -1.0f, -1.0f));
		v[1] = Vertex(Vector3f(-1.0f,  1.0f, -1.0f));
		v[2] = Vertex(Vector3f( 1.0f,  1.0f, -1.0f));
		v[3] = Vertex(Vector3f( 1.0f, -1.0f, -1.0f));
		v[4] = Vertex(Vector3f(-1.0f, -1.0f,  1.0f));
		v[5] = Vertex(Vector3f(-1.0f,  1.0f,  1.0f));
		v[6] = Vertex(Vector3f( 1.0f,  1.0f,  1.0f));
		v[7] = Vertex(Vector3f( 1.0f, -1.0f,  1.0f));
		d3d9::CHECK_HR = vb_->Unlock();

		WORD* i = nullptr;
		d3d9::CHECK_HR = ib_->Lock(0, 0, (void**)&i, 0);
		WORD n[36] = {
			0, 1, 2,//front
			0, 2, 3,
			4, 6, 5,//back
			4, 7, 6,
			4, 5, 1,//left
			4, 1, 0,
			3, 2, 6,//right
			3, 6, 7,
			1, 5, 6,//top
			1, 6, 2,
			4, 0, 3,//bottom
			4, 3, 7
		};
		memcpy(i, &n, sizeof(n));
		d3d9::CHECK_HR = ib_->Unlock();

		// camera
		D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
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
		// spin cube
		{
			D3DXMATRIX rx, ry;

			// rotate 45' on x-axis
			D3DXMatrixRotationX(&rx, angle2radian(45));

			// increment y rotation angle each frame
			static float y = 0.0f;
			D3DXMatrixRotationY(&ry, y);
			y += timestep.GetSeconds();
			if (y > 2 * M_PI)
			{
				y = 0.0f;
			}

			// combine x and y axis rotation
			D3DXMATRIX r;
			D3DXMatrixMultiply(&r, &rx, &ry);

			d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &r);
		}

		// draw cube
		d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();
		
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetStreamSource(0, vb_, 0, sizeof(Vertex));
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetIndices(ib_);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetFVF(Vertex::FVF);
		d3d9::CHECK_HR = d3d9::g_pD3DD->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

		d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
		d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(vb_);
		SAFE_RELEASE(ib_);
	}

private:
	IDirect3DVertexBuffer9* vb_;
	IDirect3DIndexBuffer9*  ib_;
};

int main()
{
	Cube app;
	app.Create(640, 480, "Cube");
	app.Loop();
	return 0;
}
