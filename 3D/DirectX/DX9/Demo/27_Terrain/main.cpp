#include "sims.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix33.h"
#include "math/matrix44.h"
#include "core/key_event.h"
#include "core/input_state.h"
#include "core/log.h"
#include "utils/fps.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"

#include "camera.h"
#include "terrain.h"
using namespace sims;

class TerrainApp : public DemoApp<dx9::Window>
{
public:
	TerrainApp()
		: terrain_(nullptr)
		, camera_(Camera::LandObject)
	{

	}

	virtual void OnCreate()
	{
		terrain_ = new Terrain(dx9::g_pD3DD,
			"coastMountain64.raw",
			0.5f,
			640,
			640,
			64,
			64);
		terrain_->GenerateTexture(Vector3f(0.0f, -1.0f, 0.0f));

		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnUpdate(const Timestep& timestep)
	{
		static float standAngle = 0.0f;

		camera_.OnUpdate(timestep);

		Vector3f eye = camera_.GetEye();
		float height = terrain_->GetHeight(eye.x, eye.z);
		eye.y = height + 15.0f; // add height because we're standing up
		camera_.SetEye(eye);

#if 0
		float currStandAngle = terrain_->ComputeStandAngle(eye.x, eye.z);
		if (!equal_t(standAngle, currStandAngle))
		{
			LOG_INFO("%f -> %f rotate angle:%f", standAngle, currStandAngle, currStandAngle - standAngle);
			auto& look = camera_.GetLook();
			LOG_INFO("%f,%f,%f", look.x, look.y, look.z);

			camera_.Pitch(angle2radian(currStandAngle - standAngle));
			standAngle = currStandAngle;
		}
#endif
	}

	virtual void OnRender(const Timestep& timestep)
	{
		D3DXMATRIX view;
		camera_.GetViewMatrix(&view);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// draw scene
		dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
		dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);
		if (terrain_)
			terrain_->Draw(I, false);

		dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
		dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
	}

	virtual void OnDestroy()
	{
		SAFE_DELETE(terrain_);
	}
private:
	Camera camera_;
	Terrain* terrain_;
};

int main()
{
	TerrainApp app;
	app.Create(640, 480, "Terrain");
	app.Loop();
	return 0;
}