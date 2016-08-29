
/********************************************************************
	created:	2016/07/09
	created:	9:7:2016   2:22
	filename: 	D:\Code\TinyRay\TinyRay\shape\sphere.h
	filename: 	D:\Code\TinyRay\TinyRay\shape
	file base:	sphere
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	
*********************************************************************/
#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "shape.h"
#include "core/ray.h"
#include "core/rgb.h"
#include "math/vector3.h"

namespace tinyray
{
	class Sphere : public Shape
	{
	public:
		Vector3f center_;
		float radius_;
		RGB color_;

		Sphere(const Vector3f& center, float radius, const RGB& color);

		bool Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const;
		bool ShadowHit(const Ray& ray, float tmin, float tmax, float time) const;
		BBox BoundingBox() const;
	};
}

#endif