#include "sims.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "core/key_event.h"
#include "utils/demo_app.h"
#include "utils/geometry_gen.h"
#include "graphics_api/sims_sdk_d3d9.h"
using namespace sims;

class TexAlpha : public DemoApp<d3d9::Window>
{
public:
	struct Vertex
	{
		Vector3f p;
		Vector2f uv;

		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
	};

	TexAlpha()
		: vbBK_(nullptr)
		, vbBox_(nullptr)
		, ibBox_(nullptr)
		, texBK_(nullptr)
		, texCrate_(nullptr)
	{}

	virtual void OnCreate()
	{
		DemoApp<d3d9::Window>::OnCreate();
		// create and fill buffer

		// BK
		{
			d3d9::CHECK_HR = d3d9::g_pD3DD->CreateVertexBuffer(6 * sizeof(Vertex),
				D3DUSAGE_WRITEONLY,
				Vertex::FVF,
				D3DPOOL_MANAGED,
				&vbBK_,
				0);

			Vertex n[6];
			GeometryGen::VBDesc vbDesc((uint8*)&n[0], sizeof(Vertex));
			vbDesc.SetVOffset(GeometryGen::VT_UV, offsetof(Vertex, uv));
			GeometryGen::GenQuad(20, 20, 5.0f, vbDesc, GeometryGen::VT_Position | GeometryGen::VT_UV);

			Vertex* v = nullptr;
			d3d9::CHECK_HR = vbBK_->Lock(0, 0, (void**)&v, 0);
			memcpy(v, n, sizeof(n));
			d3d9::CHECK_HR = vbBK_->Unlock();
		}
		// Box
		{
			d3d9::CHECK_HR = d3d9::g_pD3DD->CreateVertexBuffer(24 * sizeof(Vertex),
				D3DUSAGE_WRITEONLY,
				Vertex::FVF,
				D3DPOOL_MANAGED,
				&vbBox_,
				0);

			Vertex n[24];
			WORD i[36];

			GeometryGen::VBDesc vbDesc((uint8*)(&n[0]), sizeof(Vertex));
			vbDesc.SetVOffset(GeometryGen::VT_UV, offsetof(Vertex, uv));
			GeometryGen::GenBox(2, 2, 2, vbDesc, GeometryGen::IBDesc((uint8*)&i[0], GeometryGen::IBDesc::Index16), GeometryGen::VT_Position | GeometryGen::VT_UV);

			Vertex* v = nullptr;
			d3d9::CHECK_HR = vbBox_->Lock(0, 0, (void**)&v, 0);
			memcpy(v, n, sizeof(n));
			d3d9::CHECK_HR = vbBox_->Unlock();

			d3d9::CHECK_HR = d3d9::g_pD3DD->CreateIndexBuffer(36 * sizeof(WORD),
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&ibBox_,
				0);
			WORD* p = 0;
			d3d9::CHECK_HR = ibBox_->Lock(0, 0, (void**)&p, 0);
			memcpy(p, i, sizeof(i));
			d3d9::CHECK_HR = ibBox_->Unlock();
		}

		// crate texture
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD, "cratewalpha.dds", &texCrate_);
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD, "lobbyxpos.jpg", &texBK_);

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// alpha blending
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		// close lighting
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, false);

		// view
		D3DXVECTOR3 pos(0.0f, 0.0f, -2.5f);
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

	virtual void OnRender(const Timestep& timestep)
	{
		if (d3d9::g_pD3DD)
		{
			D3DXMATRIX rx;
			D3DXMatrixRotationX(&rx, D3DX_PI * 0.2f);

			static float y = 0.0f;
			D3DXMATRIX ry;
			D3DXMatrixRotationY(&ry, y);
			y += timestep.GetSeconds();
			if (y >= 2 * D3DX_PI)
				y = 0.0f;

			D3DXMATRIX r = rx * ry;

			// render
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff0000ff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			// draw BK
			D3DXMATRIX m;
			D3DXMatrixIdentity(&m);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetStreamSource(0, vbBK_, 0, sizeof(Vertex));
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetFVF(Vertex::FVF);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, texBK_);
			d3d9::CHECK_HR = d3d9::g_pD3DD->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

			// draw box
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &r);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetStreamSource(0, vbBox_, 0, sizeof(Vertex));
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, texCrate_);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetIndices(ibBox_);
			d3d9::CHECK_HR = d3d9::g_pD3DD->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(vbBK_);
		SAFE_RELEASE(vbBox_);
		SAFE_RELEASE(ibBox_);
		SAFE_RELEASE(texBK_);
		SAFE_RELEASE(texCrate_);
	}

private:
	IDirect3DVertexBuffer9* vbBK_;
	IDirect3DVertexBuffer9* vbBox_;
	IDirect3DIndexBuffer9* ibBox_;

	IDirect3DTexture9* texBK_;
	IDirect3DTexture9* texCrate_;
};

int main()
{
	TexAlpha app;
	app.Create(640, 480, "Tex Alpha");
	app.Loop();
	return 0;
}