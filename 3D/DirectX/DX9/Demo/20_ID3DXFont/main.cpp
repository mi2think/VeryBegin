#include "sims.h"
#include "core/log.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "utils/demo_app.h"
#include "utils/fps.h"
using namespace sims;

class ID3DXFontApp : public DemoApp<d3d9::Window>
{
public:
	ID3DXFontApp() : font_(nullptr) {}

	virtual void OnCreate()
	{
		DemoApp<d3d9::Window>::OnCreate();

		D3DXFONT_DESC desc;
		memset(&desc, 0, sizeof(desc));

		desc.Height = 25; // in logical units
		desc.Width = 12;
		desc.Weight = 500; // boldness, range 0(light) - 1000(bold)
		desc.Italic = FALSE;
		desc.CharSet = DEFAULT_CHARSET;
		desc.OutputPrecision = 0;
		desc.Quality = 0;
		desc.PitchAndFamily = 0;
		strncpy_s(desc.FaceName, "Times New Roman", sizeof(desc.FaceName));

		d3d9::CHECK_HR = D3DXCreateFontIndirect(d3d9::g_pD3DD, &desc, &font_);
	}

	virtual void OnUpdate(const Timestep& timestep)
	{
		fps_.OnUpdate(timestep);
	}

	virtual void OnRender(const Timestep&)
	{
		if (d3d9::g_pD3DD)
		{
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			RECT rect{ 0, 0, width_, height_ };
			font_->DrawText(nullptr,
				str_format("%f", fps_.Count()),
				-1,
				&rect,
				DT_TOP | DT_LEFT,
				0xff000000);

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(font_);
	}
private:
	ID3DXFont* font_;
	Fps fps_;
};

int main()
{
	ID3DXFontApp app;
	app.Create(640, 480, "ID3DXFont");
	app.Loop();
	return 0;
}