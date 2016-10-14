#include "sims.h"
#include "math/vector3.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_ogl.h"
using namespace sims;

class HelloDot : public DemoApp<ogl::Window>
{
public:
	virtual void OnCreate()
	{
		// create vertex buffer
		Vector3f vertices[1] = { Vector3f(0.0f, 0.0f, 0.0f) };

		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}

	virtual void OnRender(const Timestep&)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnableVertexAttribArray(0);
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_POINTS, 0, 1);

		glDisableVertexAttribArray(0);

		glutSwapBuffers();
	}

	virtual void OnDestroy()
	{
		glDeleteBuffers(1, &vbo_);
	}
private:
	GLuint vbo_;
};

int main()
{
	HelloDot app;
	app.Create(640, 480, "Hello Dot");
	app.Loop();
	return 0;
}
