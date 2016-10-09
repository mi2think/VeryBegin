#include "sims.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx11.h"
using namespace sims;

class InitD3D : public DemoApp<dx11::Window>
{
public:
	virtual void OnRender(const Timestep&)
	{
		const float blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };

		dx11::g_pD3DDC->ClearRenderTargetView(dx11::g_pRenderTargetView, blue);
		dx11::g_pD3DDC->ClearDepthStencilView(dx11::g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		dx11::CHECK_HR = dx11::g_pSwapChain->Present(0, 0);
	}
};

int main()
{
	InitD3D app;
	app.Create(800, 600, "Init D3D");
	app.Loop();
	return 0;
}
