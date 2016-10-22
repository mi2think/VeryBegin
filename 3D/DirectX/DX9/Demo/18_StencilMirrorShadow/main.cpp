#include "sims.h"
#include "core/key_event.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_d3d9.h"
using namespace sims;

class StencilMirrorShadow : public DemoApp<d3d9::Window>
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
		DemoApp<d3d9::Window>::OnCreate();

		// create geometry
		wallL_ = new d3d9::GeoQuad<d3d9::VertexUVN>(5.0f, 5.0f, 0.0f, Matrix44f().Identity().SetTranslation(Vector3f(-5.0f, 2.5f, 0.0f)));
		wallR_ = new d3d9::GeoQuad<d3d9::VertexUVN>(5.0f, 5.0f, 0.0f, Matrix44f().Identity().SetTranslation(Vector3f(5.0f, 2.5f, 0.0f)));
		mirror_ = new d3d9::GeoQuad<d3d9::VertexUVN>(5.0f, 5.0f, 0.0, Matrix44f().Identity().SetTranslation(Vector3f(0.0f, 2.5f, 0.0f)));
		floor_ = new d3d9::GeoPlane<d3d9::VertexUVN>(15.0f, 10.0f, 2, 2, Matrix44f().Identity().SetTranslation(Vector3f(0.0f, 0.0f, -5.0f)));
		d3d9::CHECK_HR = D3DXCreateTeapot(d3d9::g_pD3DD, &teapot_, 0);

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

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetLight(0, &light);
		d3d9::CHECK_HR = d3d9::g_pD3DD->LightEnable(0, true);

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_SPECULARENABLE, true);

		// load textures
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD, "checker.jpg", &texChecker_);
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD, "brick0.jpg", &texBrick_);
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD, "ice.bmp", &texIce_);

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// view
		D3DXVECTOR3 pos(-10.0f, 3.0f, -15.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &pos, &target, &up);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (d3d9::g_pD3DD)
		{
			ts_ = timestep.GetSeconds();

			D3DXVECTOR3 position(cosf(angle_) * radius_, 3.0f, sinf(angle_) * radius_);
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
			D3DXMATRIX view;
			D3DXMatrixLookAtLH(&view, &position, &target, &up);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xffffffff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			RenderScene();

			// render shadow before mirror, for render mirror will clear depth buffer
			RenderShadow();

			RenderMirror();

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	void RenderScene()
	{
		D3DXMATRIX m;

		// draw teapot
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetMaterial(&teapotMat_);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, 0);
		D3DXMatrixTranslation(&m, teapotPos_.x, teapotPos_.y, teapotPos_.z);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);
		d3d9::CHECK_HR = teapot_->DrawSubset(0);

		D3DXMatrixIdentity(&m);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);

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
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILENABLE, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILREF, 0);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);

		// shadow position
		D3DXVECTOR4 lightDir(0.707f, -0.707f, 0.707f, 0.0f);
		D3DXPLANE groundPlane(0.0f, -1.0f, 0.0f, 0.0f);
		D3DXMATRIX s;
		D3DXMatrixShadow(&s, &lightDir, &groundPlane);
		D3DXMATRIX t;
		D3DXMatrixTranslation(&t, teapotPos_.x, teapotPos_.y, teapotPos_.z);
		D3DXMATRIX m = t * s;
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);

		// alpha blend shadow
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		D3DMATERIAL9 mtrl;
		GenMaterial(mtrl, black, black, black, black, 0.0f);
		mtrl.Diffuse.a = 0.5f; // 50% transparency

							   // disable depth buffer so z-fighting do not occur when we render shadow on top of floor
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ZENABLE, false);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetMaterial(&mtrl);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, 0);
		teapot_->DrawSubset(0);

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ZENABLE, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILENABLE, false);
	}

	void RenderMirror()
	{
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILENABLE, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS); // always success
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILREF, 0x1); // when success, write 1 to stencil buffer
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

		// disable write to depth and back buffers
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ZWRITEENABLE, false);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// draw mirror for mark stencil region to 1
		D3DXMATRIX m;
		D3DXMatrixIdentity(&m);
		mirror_->Draw(&m, &whiteMat_, texIce_);

		// draw teapot

		// mark =1 of stencil need render
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ZWRITEENABLE, true);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

		// clear z buffer, and blend reflected teapot with mirror
		d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

		D3DXMATRIX rxy;
		D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // x-y plane
		D3DXMatrixReflect(&rxy, &plane);
		D3DXMATRIX trans;
		D3DXMatrixTranslation(&trans, teapotPos_.x, teapotPos_.y, teapotPos_.z);
		m = trans * rxy;
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetMaterial(&teapotMat_);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, 0);

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		teapot_->DrawSubset(0);

		// restore states
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_STENCILENABLE, false);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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
	d3d9::GeoBase* wallL_;
	d3d9::GeoBase* wallR_;
	d3d9::GeoBase* mirror_;
	d3d9::GeoBase* floor_;
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