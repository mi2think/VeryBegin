/********************************************************************
	created:	2016/08/04
	created:	4:8:2016   20:59
	filename: 	D:\Code\TinyRay\TinyRay\shape\uvsphere.h
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	uvsphere
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	UV Sphere
*********************************************************************/
#ifndef __UV_SPHERE_H__
#define __UV_SPHERE_H__

#include "shape.h"
#include "core/ray.h"
#include "core/rgb.h"
#include "core/texture.h"
#include "math/vector3.h"

namespace tinyray
{
	class UVSphere : public Shape
	{
	public:
		Vector3f center_;
		float radius_;
		Texture* texture_;

		UVSphere(const Vector3f& center, float radius, Texture* texture);

		bool Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const;
		bool ShadowHit(const Ray& ray, float tmin, float tmax, float time) const;
		BBox BoundingBox() const;
	};
}

#endif
