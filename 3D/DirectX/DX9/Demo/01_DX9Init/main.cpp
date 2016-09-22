#include "sims.h"
#include "core/log.h"
#include "graphics_api/sims_sdk_dx9.h"
#include "utils/demo_app.h"
using namespace sims;

class Dx9Init : public DemoApp<dx9::Window>
{
public:
	virtual void OnRender(const Timestep& timestep)
	{
		LOG_INFO("ts:%d", timestep.GetMilliseconds());
		if (dx9::g_pD3DD)
		{
			dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff0000ff, 1.0f, 0);
			dx9::g_pD3DD->Present(0, 0, 0, 0);
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