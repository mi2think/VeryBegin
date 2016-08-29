/********************************************************************
	created:	2016/07/09
	created:	9:7:2016   3:20
	filename: 	D:\Code\TinyRay\Demo\01_GenPPM\main.cpp
	file path:	D:\Code\TinyRay\Demo\01_GenPPM
	file base:	main
	file ext:	cpp
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	Generate PPM using orthogonal viewing ray tracer
*********************************************************************/
#include "core/rgb.h"
#include "core/image.h"
#include "shape/shape.h"
#include "shape/triangle.h"
#include "shape/sphere.h"
#include "core/ray.h"

#include <vector>

using namespace std;
using namespace tinyray;

int main()
{
	HitRecord rec;
	bool isHit;
	// max valid t parameter
	float tmax;
	// direction of viewing rays
	Vector3f dir(0, 0, 1);

	// geometry
	vector<Shape*> shapes;
	Shape* sphere = new Sphere(Vector3f(250, 250, 1000), 150, RGB(0.2f, 0.2f, 0.8f));
	Shape* triangle = new Triangle(Vector3f(300.0f, 600.0f, 800.0f), Vector3f(0.0f, 100.0f, 1000.0f), Vector3f(450.0f, 20.0f, 1000.0f), RGB(0.8f, 0.2f, 0.2f));
	shapes.push_back(sphere);
	shapes.push_back(triangle);

	int width  = 500;
	int height = 500;
	PPM im(width, height);

	// loop over pixel
	RGB* pixel = &(im.GetRaster()[0]);
	for (int y = height - 1; y >= 0; --y)
	{
		for (int x = 0; x < width; ++x, ++pixel)
		{
			tmax = 100000.0f;
			isHit = false;
			Ray ray(Vector3f(x, y, 0), dir);
			
			// loop over list of shapes
			for (const auto& shape : shapes)
			{
				float time = 0.0f;
				if (shape->Hit(ray, 0.00001f, tmax, time, rec))
				{
					tmax = rec.t;
					isHit = true;
				}
			}

			// PPM's image data from top to bottom, from left to right
			*pixel = (isHit ? rec.color : RGB(0.2f, 0.2f, 0.2f));
		}
	}
	im.SavePPM("gen_ppm.ppm");

	return 0;
}