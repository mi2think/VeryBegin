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

class DirectionalLight : public DemoApp<dx9::Window>
{
public:
	DirectionalLight()
		: angle_(3.0f * D3DX_PI / 2.0f)
		, cameraH_(5.0f)
		, ts_(0.0f)
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
		GenMaterial(mtrls_[0], red, red, red, black, 2.0f);
		// blue
		GenMaterial(mtrls_[1], blue, blue, blue, black, 2.0f);
		// green
		GenMaterial(mtrls_[2], green, green, green, black, 2.0f);
		// yellow
		GenMaterial(mtrls_[3], yellow, yellow, yellow, black, 2.0f);

		// setup directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = white;
		light.Specular = white * 0.6f;
		light.Ambient = white * 0.6f;
		light.Direction = D3DXVECTOR3(1.0f, -0.0f, 0.25f);

		// set and enable light
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetLight(0, &light);
		dx9::CHECK_HR = dx9::g_pD3DD->LightEnable(0, true);

		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SPECULARENABLE, false);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual bool OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &DirectionalLight::OnKeyEvent);
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

	virtual void OnRender(const Timestep& timestep)
	{
		ts_ = timestep.GetSeconds();

		// view
		D3DXVECTOR3 position(cosf(angle_) * 10.0f, cameraH_, sinf(angle_) * 10.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &position, &target, &up);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

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

	float angle_;
	float cameraH_;
	float ts_; // last frame second
};

int main()
{
	DirectionalLight app;
	app.Create(640, 480, "Directional Light");
	app.Loop();
	return 0;
}
