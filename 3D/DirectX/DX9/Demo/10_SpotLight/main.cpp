#include "sims.h"
#include "core/log.h"
#include "core/key_event.h"
#include "graphics_api/sims_sdk_dx9.h"
#include "utils/demo_app.h"
using namespace sims;

#define red   D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)
#define green D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)
#define blue  D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)
#define white D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
#define black D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f)
#define yellow D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f)

#define GenMaterial(m, a, d, s, e, p) { m.Ambient = a; m.Diffuse = d, m.Specular = s; m.Emissive = e; m.Power = p; } 

class SpotLight : public DemoApp<dx9::Window>
{
public:
	SpotLight()
		: ts_(0.0f)
	{
		memset(objs_, 0, sizeof(objs_));
	}

	virtual void OnCreate()
	{
		DemoApp<dx9::Window>::OnCreate();

		// create obj
		dx9::CHECK_HR = D3DXCreateTeapot(dx9::g_pD3DD, &objs_[0], 0);
		dx9::CHECK_HR = D3DXCreateSphere(dx9::g_pD3DD, 1.0f, 20, 20, &objs_[1], 0);
		dx9::CHECK_HR = D3DXCreateTorus(dx9::g_pD3DD, 0.5f, 1.0f, 20, 20, &objs_[2], 0);
		dx9::CHECK_HR = D3DXCreateCylinder(dx9::g_pD3DD, 0.5f, 0.5f, 2.0f, 20, 20, &objs_[3], 0);

		// build world matrix
		D3DXMatrixTranslation(&mtxs_[0], 0.0f, 2.0f, 0.0f);
		D3DXMatrixTranslation(&mtxs_[1], 0.0f, -2.0f, 0.0f);
		D3DXMatrixTranslation(&mtxs_[2], -3.0f, 0.0f, 0.0f);
		D3DXMatrixTranslation(&mtxs_[3], 3.0f, 0.0f, 0.0f);

		// setup material
		// red
		GenMaterial(mtrls_[0], red, red, red, black, 20.0f);
		// blue
		GenMaterial(mtrls_[1], blue, blue, blue, black, 20.0f);
		// green
		GenMaterial(mtrls_[2], green, green, green, black, 20.0f);
		// yellow
		GenMaterial(mtrls_[3], yellow, yellow, yellow, black, 20.0f);

		// setup spot light_
		memset(&light_, 0, sizeof(light_));
		light_.Type = D3DLIGHT_SPOT;
		light_.Diffuse = white;
		light_.Specular = white * 0.6f;
		light_.Position = D3DXVECTOR3(0.0f, 0.0f, -5.0f);
		light_.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		light_.Range = 1000.0f;
		light_.Falloff = 1.0f;
		light_.Attenuation0 = 1.0f;
		light_.Attenuation1 = 0.0f;
		light_.Attenuation2 = 0.0f;
		light_.Theta = 0.4f;
		light_.Phi = 0.9f;

		// set and enable light_
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetLight(0, &light_);
		dx9::CHECK_HR = dx9::g_pD3DD->LightEnable(0, true);

		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SPECULARENABLE, true);

		// view
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

	virtual bool OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &SpotLight::OnKeyEvent);
		return dispatch.GetResult();
	}

	bool OnKeyEvent(const KeyPressEvent& event)
	{
		int key = event.GetKey();
		switch (key)
		{
		case KEY_LEFT:
			light_.Direction.x -= ts_ * 0.5f;
			break;
		case KEY_RIGHT:
			light_.Direction.x += ts_ * 0.5f;
			break;
		case KEY_UP:
			light_.Direction.y -= ts_ * 0.5f;
			break;
		case KEY_DOWN:
			light_.Direction.y += ts_ * 0.5f;
			break;
		default:
			break;
		}

		dx9::CHECK_HR = dx9::g_pD3DD->SetLight(0, &light_);
		dx9::CHECK_HR = dx9::g_pD3DD->LightEnable(0, true);
		return true;
	}

	virtual void OnRender(const Timestep& timestep)
	{
		ts_ = timestep.GetSeconds();

		// draw scene
		dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

		for (int i = 0; i < MAX_OBJS; ++i)
		{
			dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&mtrls_[i]);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &mtxs_[i]);
			dx9::CHECK_HR = objs_[i]->DrawSubset(0);
		}

		dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
		dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
	}

	virtual void OnDestroy()
	{
		for (auto& obj : objs_)
		{
			SAFE_RELEASE(obj);
		}
	}

	enum { MAX_OBJS = 4 };
private:
	ID3DXMesh* objs_[MAX_OBJS];
	D3DXMATRIX mtxs_[MAX_OBJS];
	D3DMATERIAL9 mtrls_[MAX_OBJS];
	D3DLIGHT9 light_;

	float ts_; // last frame second
};

int main()
{
	SpotLight app;
	app.Create(640, 480, "Spot Light");
	app.Loop();
	return 0;
}
