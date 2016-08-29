/********************************************************************
	created:	2016/07/08
	created:	8:7:2016   23:37
	filename: 	D:\Code\TinyRay\TinyRay\shape\triangle.h
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	triangle
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:
		Hit using "Fast Minimum Storage Ray Triangle Intersection"

	purpose:	
*********************************************************************/
#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "shape.h"
#include "core/ray.h"
#include "core/rgb.h"
#include "math/vector3.h"

namespace tinyray
{
	class Triangle : public Shape
	{
	public:
		Vector3f p0_;
		Vector3f p1_;
		Vector3f p2_;
		RGB color_;

		Triangle(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const RGB& color);

		bool Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const;
		bool ShadowHit(const Ray& ray, float tmin, float tmax, float time) const;
		BBox BoundingBox() const;
	};
}


#endif