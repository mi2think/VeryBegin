#include "sims.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_d3d11.h"
using namespace sims;

class InitD3D : public DemoApp<d3d11::Window>
{
public:
	virtual void OnRender(const Timestep&)
	{
		const float blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };

		d3d11::g_pD3DDC->ClearRenderTargetView(d3d11::g_pRenderTargetView, blue);
		d3d11::g_pD3DDC->ClearDepthStencilView(d3d11::g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		d3d11::CHECK_HR = d3d11::g_pSwapChain->Present(0, 0);
	}
};

int main()
{
	InitD3D app;
	app.Create(800, 600, "Init D3D");
	app.Loop();
	return 0;
}
