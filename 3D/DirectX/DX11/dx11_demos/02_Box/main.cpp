#include "sims.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx11.h"
using namespace sims;

class BoxApp : public DemoApp<dx11::Window>
{
public:
	BoxApp()
		: box_(nullptr)
	{}

	virtual void OnCreate()
	{

	}

	virtual void OnRender(const Timestep&)
	{
		dx11::g_pD3DDC->ClearRenderTargetView(dx11::g_pRenderTargetView, (const float*)&dx11::LightSteelBlue);
		dx11::g_pD3DDC->ClearDepthStencilView(dx11::g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		dx11::CHECK_HR = dx11::g_pSwapChain->Present(0, 0);
	}

private:
	dx11::GeoBox<dx11::Vertex>* box_;
};

int main()
{
	BoxApp app;
	app.Create(800, 600, "Box");
	app.Loop();
	return 0;
}