#include "sims.h"
#include "core/key_event.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class StencilMirrorShadow : public DemoApp<dx9::Window>
{
public:
	StencilMirrorShadow()
		: wallL_(nullptr)
		, wallR_(nullptr)
		, mirror_(nullptr)
		, floor_(nullptr)
		, teapot_(nullptr)
		, texChecker_(nullptr)
		, texIce_(nullptr)
		, ts_(0.0f)
		, radius_(20.0f)
		, angle_(3.0f * D3DX_PI / 2.0f)
	{}

	~StencilMirrorShadow()
	{
		delete wallL_;
		delete wallR_;
		delete mirror_;
		delete floor_;
	}

	virtual void OnCreate()
	{
		DemoApp<dx9::Window>::OnCreate();

		// create geometry
		wallL_ = new dx9::GeoQuad<dx9::VertexUVN>(5.0f, 5.0f, 0.0f, Matrix44f().Identity().SetTranslation(Vector3f(-5.0f, 2.5f, 0.0f)));
		wallR_ = new dx9::GeoQuad<dx9::VertexUVN>(5.0f, 5.0f, 0.0f, Matrix44f().Identity().SetTranslation(Vector3f(5.0f, 2.5f, 0.0f)));
		mirror_ = new dx9::GeoQuad<dx9::VertexUVN>(5.0f, 5.0f, 0.0, Matrix44f().Identity().SetTranslation(Vector3f(0.0f, 2.5f, 0.0f)));
		floor_ = new dx9::GeoPlane<dx9::VertexUVN>(15.0f, 10.0f, 2, 2, Matrix44f().Identity().SetTranslation(Vector3f(0.0f, 0.0f, -5.0f)));
		dx9::CHECK_HR = D3DXCreateTeapot(dx9::g_pD3DD, &teapot_, 0);

		teapotPos_.x = 0.0f;
		teapotPos_.y = 3.0f;
		teapotPos_.z = -7.5f;

		// material
		GenMaterial(teapotMat_, yellow, yellow, yellow, black, 2.0f);
		GenMaterial(whiteMat_, white, white, white, black, 2.0f);
		GenMaterial(wallMat_, white, white, white * 0.2f, black, 2.0f);

		// setup directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = white;
		light.Specular = white * 0.6f;
		light.Ambient = white * 0.4f;
		light.Direction = D3DXVECTOR3(0.707f, -0.707f, 0.707f);

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

			// render shadow before mirror, for render mirror will clear depth buffer
			RenderShadow();

			RenderMirror();

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
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
		wallL_->Draw(nullptr, &wallMat_, texBrick_);
		wallR_->Draw(nullptr, &wallMat_, texBrick_);

		// draw mirror
		mirror_->Draw(nullptr, &whiteMat_, texIce_);

		// draw floor
		floor_->Draw(nullptr, &whiteMat_, texChecker_);
	}

	void RenderShadow()
	{
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILENABLE, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILREF, 0);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);

		// shadow position
		D3DXVECTOR4 lightDir(0.707f, -0.707f, 0.707f, 0.0f);
		D3DXPLANE groundPlane(0.0f, -1.0f, 0.0f, 0.0f);
		D3DXMATRIX s;
		D3DXMatrixShadow(&s, &lightDir, &groundPlane);
		D3DXMATRIX t;
		D3DXMatrixTranslation(&t, teapotPos_.x, teapotPos_.y, teapotPos_.z);
		D3DXMATRIX m = t * s;
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);

		// alpha blend shadow
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		D3DMATERIAL9 mtrl;
		GenMaterial(mtrl, black, black, black, black, 0.0f);
		mtrl.Diffuse.a = 0.5f; // 50% transparency

							   // disable depth buffer so z-fighting do not occur when we render shadow on top of floor
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ZENABLE, false);
		dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&mtrl);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, 0);
		teapot_->DrawSubset(0);

		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ZENABLE, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_STENCILENABLE, false);
	}

	void RenderMirror()
	{
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
		D3DXMATRIX m;
		D3DXMatrixIdentity(&m);
		mirror_->Draw(&m, &whiteMat_, texIce_);

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

	virtual bool OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &StencilMirrorShadow::OnKeyEvent);
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

	virtual void OnDestroy()
	{
		wallL_->OnDestory();
		wallR_->OnDestory();
		mirror_->OnDestory();
		floor_->OnDestory();
	}

private:
	dx9::GeoBase* wallL_;
	dx9::GeoBase* wallR_;
	dx9::GeoBase* mirror_;
	dx9::GeoBase* floor_;
	ID3DXMesh* teapot_;
	IDirect3DTexture9* texChecker_;
	IDirect3DTexture9* texBrick_;
	IDirect3DTexture9* texIce_;
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
	StencilMirrorShadow app;
	app.Create(640, 480, "Stencil Mirror Shadow");
	app.Loop();
	return 0;
}