#include "sims.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "core/key_event.h"
#include "graphics_api/sims_dx9.h"
#include "utils/geometry_gen.h"
#include "utils/demo_app.h"
using namespace sims;

#define white D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
#define black D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)

#define GenMaterial(m, a, d, s, e, p) { m.Ambient = a; m.Diffuse = d, m.Specular = s; m.Emissive = e; m.Power = p; } 

class TexCube : public DemoApp<dx9::Window>
{
public:
	struct Vertex
	{
		Vector3f pos;
		Vector3f normal;
		Vector2f uv;
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

		Vertex() {}
		Vertex(const Vector3f& p) : pos(p) {}
	};

	TexCube()
		: vb_(nullptr)
		, ib_(nullptr)
		, tex_(nullptr)
		, angle_(3.0f * D3DX_PI / 2.0f)
		, cameraH_(2.0f)
		, ts_(0.0f)
	{}

	virtual void OnCreate()
	{
		DemoApp<dx9::Window>::OnCreate();

		// create vertex buffer and index buffer
		dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexBuffer(14 * sizeof(Vertex),
			D3DUSAGE_WRITEONLY,
			Vertex::FVF,
			D3DPOOL_MANAGED,
			&vb_,
			0);
		dx9::CHECK_HR = dx9::g_pD3DD->CreateIndexBuffer(6 * 2 * 3 * sizeof(WORD),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&ib_,
			0);

		Vertex v[14];
		WORD i[36];
		GeometryGen::VBDesc vbDesc((uint8*)&v, sizeof(Vertex));
		vbDesc.SetVOffset(GeometryGen::VT_UV, offsetof(Vertex, uv));
		vbDesc.SetVOffset(GeometryGen::VT_Normal, offsetof(Vertex, normal));
		GeometryGen::GenUVCube(2, vbDesc, GeometryGen::IBDesc((uint8*)&i, GeometryGen::IBDesc::Index16), GeometryGen::VT_Position | GeometryGen::VT_Normal | GeometryGen::VT_UV);

		// fill buffer
		Vertex* _v = nullptr;
		dx9::CHECK_HR = vb_->Lock(0, 0, (void**)&_v, 0);
		memcpy(_v, v, sizeof(v));
		dx9::CHECK_HR = vb_->Unlock();

		WORD* _i = nullptr;
		dx9::CHECK_HR = ib_->Lock(0, 0, (void**)&_i, 0);
		memcpy(_i, i, sizeof(i));
		dx9::CHECK_HR = ib_->Unlock();

		// texture
		dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD, "sky.png", &tex_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, tex_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// setup directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
		light.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
		light.Direction = D3DXVECTOR3(1.0f, -1.0f, 0.0f);

		// set and enable light
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetLight(0, &light);
		dx9::CHECK_HR = dx9::g_pD3DD->LightEnable(0, true);

		// material
		GenMaterial(material_, white, white, white, black, 2.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&material_);

		// camera
		D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
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
		ts_ = timestep.GetSeconds();

		// view
		D3DXVECTOR3 position(cosf(angle_) * 3.0f, cameraH_, sinf(angle_) * 3.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &position, &target, &up);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// draw cube
		dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

		dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, vb_, 0, sizeof(Vertex));
		dx9::CHECK_HR = dx9::g_pD3DD->SetIndices(ib_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetFVF(Vertex::FVF);
		dx9::CHECK_HR = dx9::g_pD3DD->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 14, 0, 12);

		dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
		dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
	}

	virtual bool OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &TexCube::OnKeyEvent);
		return dispatch.GetResult();
	}

	bool OnKeyEvent(const KeyPressEvent& event)
	{
		int key = event.GetKey();
		switch (key)
		{
		case KEY_LEFT:
			angle_ -= ts_ * 0.5f;
			break;
		case KEY_RIGHT:
			angle_ += ts_ * 0.5f;
			break;
		case KEY_UP:
			cameraH_ += ts_ * 5.0f;
			break;
		case KEY_DOWN:
			cameraH_ -= ts_ * 5.0f;
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
	}

private:
	IDirect3DVertexBuffer9* vb_;
	IDirect3DIndexBuffer9*  ib_;
	IDirect3DTexture9*		tex_;
	D3DMATERIAL9 material_;

	float angle_;
	float cameraH_;
	float ts_; // last frame second
};

int main()
{
	TexCube app;
	app.Create(640, 480, "Tex Cube");
	app.Loop();
	return 0;
}