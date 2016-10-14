#include "sims.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_ogl.h"
using namespace sims;

class OpenAWindow : public DemoApp<ogl::Window>
{
public:
	virtual void OnRender(const Timestep&)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();
	}
};

int main()
{
	OpenAWindow app;
	app.Create(640, 480, "Open A Window");
	app.Loop();
	return 0;
}