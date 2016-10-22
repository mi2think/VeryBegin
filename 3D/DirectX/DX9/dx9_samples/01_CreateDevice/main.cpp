#include "sims.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_d3d9.h"
using namespace sims;

class CreateDevice : public DemoApp<d3d9::Window>
{
public:
	virtual void OnRender(const Timestep&)
	{
		if (d3d9::g_pD3DD)
		{
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET, 0xff0000ff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}
};

int main()
{
	CreateDevice app;
	app.Create(300, 300, "Create Device");
	app.Loop();
	return 0;
}

