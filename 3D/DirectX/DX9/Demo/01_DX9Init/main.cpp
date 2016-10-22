#include "sims.h"
#include "core/log.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "utils/demo_app.h"
using namespace sims;

class Dx9Init : public DemoApp<d3d9::Window>
{
public:
	virtual void OnRender(const Timestep& timestep)
	{
		LOG_INFO("ts:%d", timestep.GetMilliseconds());
		if (d3d9::g_pD3DD)
		{
			d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff0000ff, 1.0f, 0);
			d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}
};

int main()
{
	Dx9Init app;
	app.Create(640, 480, "Dx9 Init");
	app.Loop();

	return 0;
}