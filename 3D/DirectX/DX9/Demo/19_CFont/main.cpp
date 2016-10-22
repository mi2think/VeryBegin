#include "sims.h"
#include "core/log.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "utils/demo_app.h"

#include "d3dutil.h"
#include "d3dfont.h"

using namespace sims;

class CFont : public DemoApp<d3d9::Window>
{
public:
	CFont() 
		: font_(nullptr)
		, frameCount_(0)
		, timeElapsed_(0.0f)
		, fps_(0)
		{}

	virtual void OnCreate()
	{
		DemoApp<d3d9::Window>::OnCreate();

		font_ = new CD3DFont("Times New Roman", 16, 0);
		font_->InitDeviceObjects(d3d9::g_pD3DD);
		font_->RestoreDeviceObjects();
	}

	virtual void OnDestroy()
	{
		if (font_)
		{
			font_->InvalidateDeviceObjects();
			font_->DeleteDeviceObjects();
			SAFE_DELETE(font_);
		}
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (d3d9::g_pD3DD)
		{
			++frameCount_;
			timeElapsed_ += timestep.GetSeconds();
			if (timeElapsed_ >= 1.0f)
			{
				fps_ = frameCount_ / timeElapsed_;
				frameCount_ = 0;
				timeElapsed_ = 0.0f;
			}

			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			if (font_)
			{
				font_->DrawText(20, 20, 0xff000000, str_format("%f", fps_));
			}

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

private:
	CD3DFont* font_;
	uint32 frameCount_;
	float timeElapsed_;
	float fps_;
};

int main()
{
	CFont app;
	app.Create(640, 480, "CFont");
	app.Loop();
	return 0;
}