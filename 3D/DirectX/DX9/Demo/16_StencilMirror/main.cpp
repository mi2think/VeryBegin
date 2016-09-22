#include "sims.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/matrix44.h"
#include "core/key_event.h"
#include "utils/demo_app.h"
#include "utils/geometry_gen.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

#define red   D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)
#define green D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)
#define blue  D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)
#define white D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
#define black D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)
#define yellow D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f)

#define GenMaterial(m, a, d, s, e, p) { m.Ambient = a; m.Diffuse = d, m.Specular = s; m.Emissive = e; m.Power = p; } 

class StencilMirror : public DemoApp<dx9::Window>
{
public:
	struct Vertex
	{
		Vector3f p;
		Vector3f n;
		Vector2f uv;

		Vertex() {}
		Vertex(const Vector3f& _p, const Vector3f& _n, const Vector2f& _uv)
			: p(_p)
			, n(_n)
			, uv(_uv)
		{}

		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	};

	StencilMirror()
		: vbWall_(nullptr)
		, vbMirror_(nullptr)
		, vbFloor_(nullptr)
		, texChecker_(nullptr)
		, texBrick_(nullptr)
		, texIce_(nullptr)
		, teapot_(nullptr)
		, ibFloor_(nullptr)
		, ts_(0.0f)
		, radius_(20.0f)
		, angle_(3.0f * D3DX_PI / 2.0f)
	{}

	virtual void OnCreate()
	{
		DemoApp<dx9::Window>::OnCreate();

		// create buffer
		// wall
		{
			// 2 quad
			Vertex n[12];

			Matrix44f m;
			m.Identity().SetTranslation(Vector3f(-5.0f, 2.5f, 0.0f));
			GeometryGen::VBDesc vbDesc((uint8*)&n[0], sizeof(Vertex), 0, offsetof(Vertex, uv), offsetof(Vertex, n));
			vbDesc.SetTransform(m);
			GeometryGen::GenQuad(5, 5, 0, vbDesc, GeometryGen::VT_Position | GeometryGen::VT_Normal | GeometryGen::VT_UV);

			vbDesc.offset = 6 * sizeof(Vertex);
			m.SetTranslation(Vector3f(5.0f, 2.5f, 0.0f));
			vbDesc.SetTransform(m);
			GeometryGen::GenQuad(5, 5, 0, vbDesc, GeometryGen::VT_Position | GeometryGen::VT_Normal | GeometryGen::VT_UV);

			dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexBuffer(sizeof(n),
				D3DUSAGE_WRITEONLY,
				Vertex::FVF,
				D3DPOOL_MANAGED,
				&vbWall_,
				0);

			Vertex* v = nullptr;
			dx9::CHECK_HR = vbWall_->Lock(0, 0, (void**)&v, 0);
			memcpy(v, &n[0], sizeof(n));
			dx9::CHECK_HR = vbWall_->Unlock();
		}
		// mirror
		{
			Vertex n[6];
			Matrix44f m;
			m.Identity().SetTranslation(Vector3f(0.0f, 2.5f, 0.0f));
			GeometryGen::VBDesc vbDesc((uint8*)&n[0], sizeof(Vertex), 0, offsetof(Vertex, uv), offsetof(Vertex, n));
			vbDesc.SetTransform(m);
			GeometryGen::GenQuad(5, 5, 0, vbDesc, GeometryGen::VT_Position | GeometryGen::VT_Normal | GeometryGen::VT_UV);

			dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexBuffer(sizeof(n),
				D3DUSAGE_WRITEONLY,
				Vertex::FVF,
				D3DPOOL_MANAGED,
				&vbMirror_,
				0);

			Vertex* v = nullptr;
			dx9::CHECK_HR = vbMirror_->Lock(0, 0, (void**)&v, 0);
			memcpy(v, &n[0], sizeof(n));
			dx9::CHECK_HR = vbMirror_->Unlock();
		}
		// floor
		{
			Vertex n[4];
			WORD i[6];

			Matrix44f m;
			m.Identity().SetTranslation(Vector3f(0.0f, 0.0f, -5.0f));
			GeometryGen::VBDesc vbDesc((uint8*)&n[0], sizeof(Vertex), 0, offsetof(Vertex, uv), offsetof(Vertex, n));
			vbDesc.SetTransform(m);
			GeometryGen::GenPlane(15, 10, 2, 2, vbDesc, GeometryGen::IBDesc((uint8*)&i[0], GeometryGen::IBDesc::Index16), GeometryGen::VT_Position | GeometryGen::VT_Normal | GeometryGen::VT_UV);

			dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexBuffer(sizeof(n),
				D3DUSAGE_WRITEONLY,
				Vertex::FVF,
				D3DPOOL_MANAGED,
				&vbFloor_,
				0);

			Vertex* v = nullptr;
			dx9::CHECK_HR = vbFloor_->Lock(0, 0, (void**)&v, 0);
			memcpy(v, &n[0], sizeof(n));
			dx9::CHECK_HR = vbFloor_->Unlock();
			
			dx9::CHECK_HR = dx9::g_pD3DD->CreateIndexBuffer(sizeof(i),
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&ibFloor_,
				0);
			WORD* w = nullptr;
			dx9::CHECK_HR = ibFloor_->Lock(0, 0, (void**)&w, 0);
			memcpy(w, &i[0], sizeof(i));
			dx9::CHECK_HR = ibFloor_->Unlock();
		}

		// teapot
		dx9::CHECK_HR = D3DXCreateTeapot(dx9::g_pD3DD, &teapot_, 0);
		GenMaterial(teapotMat_, yellow, yellow, yellow, black, 2.0f);
		teapotPos_.x = 0.0f;
		teapotPos_.y = 3.0f;
		teapotPos_.z = -7.5f;

		// setup directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = white;
		light.Specular = white * 0.6f;
		light.Ambient = white * 0.4f;
		light.Direction = D3DXVECTOR3(0.707f, -0.707f, 0.707f);

		GenMaterial(whiteMat_, white, white, white, black, 2.0f);
		GenMaterial(wallMat_, white, white, white * 0.2f, black, 2.0f);

		// set and enable light
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetLight(0, &light);
		dx9::CHECK_HR = dx9::g_pD3DD->LightEnable(0, true);

		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SPECULARENABLE, true);

		// load textures
		dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD, "checker.jpg", &texChecker_);
		dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD, "brick0.jpg", &texBrick_);
		dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD, "ice.bmp", &texIce_);

		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// view
		D3DXVECTOR3 pos(-10.0f, 3.0f, -15.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &pos, &target, &up);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			ts_ = timestep.GetSeconds();

			D3DXVECTOR3 position(cosf(angle_) * radius_, 3.0f, sinf(angle_) * radius_);
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
			D3DXMATRIX view;
			D3DXMatrixLookAtLH(&view, &position, &target, &up);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xffffffff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			RenderScene();

			RenderMirror();

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual bool OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &StencilMirror::OnKeyEvent);
		return dispatch.GetResult();
	}

	bool OnKeyEvent(const KeyPressEvent& event)
	{
		int key = event.GetKey();
		switch (key)
		{
		case KEY_LEFT:
			teapotPos_.x -= 3.0f * ts_;
			break;
		case KEY_RIGHT:
			teapotPos_.x += 3.0f * ts_;
			break;
		case KEY_UP:
			radius_ -= 2.0f * ts_;
			break;
		case KEY_DOWN:
			radius_ += 2.0f * ts_;
			break;
		case 'A':
			angle_ -= 0.5f * ts_;
			break;
		case 'D':
			angle_ += 0.5f * ts_;
			break;
		default:
			break;
		}
		return true;
	}

	void RenderScene()
	{
		D3DXMATRIX m;

		// draw teapot
		dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&teapotMat_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, 0);
		D3DXMatrixTranslation(&m, teapotPos_.x, teapotPos_.y, teapotPos_.z);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);
		dx9::CHECK_HR = teapot_->DrawSubset(0);

		D3DXMatrixIdentity(&m);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);

		// draw wall
		dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&wallMat_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, texBrick_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetFVF(Vertex::FVF);
		dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, vbWall_, 0, sizeof(Vertex));
		dx9::CHECK_HR = dx9::g_pD3DD->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);

		dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&whiteMat_);

		// draw mirror
		dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, texIce_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetFVF(Vertex::FVF);
		dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, vbMirror_, 0, sizeof(Vertex));
		dx9::CHECK_HR = dx9::g_pD3DD->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// draw floor
		dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, texChecker_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetFVF(Vertex::FVF);
		dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, vbFloor_, 0, sizeof(Vertex));
		dx9::CHECK_HR = dx9::g_pD3DD->SetIndices(ibFloor_);
		dx9::CHECK_HR = dx9::g_pD3DD->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	}

	void RenderMirror()
	{
		// draw mirror quad to stencil buffer only.
		// 0. clear stencil to 0
		// 1. draw mirror, stencil test always success, so stencil region of mirror is 1
		// 2. we do not want step 1 affect color buffer and depth buffer, so need to do some trick (using alpha blend)
		// 3. gen reflect matrix
		// 3. draw teapot 

		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILENABLE, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS); // always success
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILREF, 0x1); // when success, write 1 to stencil buffer
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

		// disable write to depth and back buffers
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ZWRITEENABLE, false);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// draw mirror for mark stencil region to 1
		dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, vbMirror_, 0, sizeof(Vertex));
		dx9::CHECK_HR = dx9::g_pD3DD->SetFVF(Vertex::FVF);
		dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&whiteMat_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, texIce_);
		D3DXMATRIX m;
		D3DXMatrixIdentity(&m);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);
		dx9::CHECK_HR = dx9::g_pD3DD->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// draw teapot

		// mark =1 of stencil need render
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ZWRITEENABLE, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

		// clear z buffer, and blend reflected teapot with mirror
		dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

		D3DXMATRIX rxy;
		D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // x-y plane
		D3DXMatrixReflect(&rxy, &plane);
		D3DXMATRIX trans;
		D3DXMatrixTranslation(&trans, teapotPos_.x, teapotPos_.y, teapotPos_.z);
		m = trans * rxy;
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);
		dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&teapotMat_);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, 0);

		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		teapot_->DrawSubset(0);

		// restore states
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILENABLE, false);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(vbWall_);
		SAFE_RELEASE(vbMirror_);
		SAFE_RELEASE(vbFloor_);
		SAFE_RELEASE(texChecker_);
		SAFE_RELEASE(texBrick_);
		SAFE_RELEASE(texIce_);
		SAFE_RELEASE(teapot_);
		SAFE_RELEASE(ibFloor_);
	}
private:
	IDirect3DVertexBuffer9* vbWall_;
	IDirect3DVertexBuffer9* vbMirror_;
	IDirect3DVertexBuffer9* vbFloor_;
	IDirect3DIndexBuffer9* ibFloor_;
	IDirect3DTexture9* texChecker_;
	IDirect3DTexture9* texBrick_;
	IDirect3DTexture9* texIce_;
	ID3DXMesh* teapot_;
	D3DMATERIAL9 teapotMat_;
	D3DMATERIAL9 whiteMat_;
	D3DMATERIAL9 wallMat_;
	D3DXVECTOR3 teapotPos_;
	float ts_; // last frame second
	float radius_;
	float angle_;
};

int main()
{
	StencilMirror app;
	app.Create(640, 480, "Stencil Mirror");
	app.Loop();
	return 0;
}