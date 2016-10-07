#include "sims.h"
#include "math/vector3.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class Lights : public DemoApp<dx9::Window>
{
public:
	struct CUSTOMVERTEX
	{
		Vector3f position;
		Vector3f normal;
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL;
	};


	Lights()
		: vb_(nullptr)
	{}

	virtual void OnCreate()
	{

	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(vb_);
	}
private:
	IDirect3DVertexBuffer9* vb_;
};

int main()
{
	Lights app;
	app.Create(300, 300, "Lights");
	app.Loop();
	return 0;
}