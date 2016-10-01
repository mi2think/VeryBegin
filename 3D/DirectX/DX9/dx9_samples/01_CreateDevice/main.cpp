#include "sims.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class CreateDevice : public DemoApp<dx9::Window>
{
public:
	virtual void OnRender(const Timestep&)
	{
		if (dx9::g_pD3DD)
		{
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET, 0xff0000ff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
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

