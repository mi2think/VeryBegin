/********************************************************************
	created:	2016/07/07
	created:	7:7:2016   22:58
	filename: 	D:\Code\TinyRay\TinyRay\shape\shape.h
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	shape
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	
*********************************************************************/
#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "core/rgb.h"
#include "core/ray.h"
#include "core/texture.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "bbox.h"

namespace tinyray
{
	struct HitRecord
	{
		float t;
		Vector3f normal;
		RGB color;
		Vector2f uv;
		Vector3f hitPos;
		Texture* tex;
	};

	class Shape
	{
	public:
		virtual ~Shape() {}
		virtual bool Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const = 0;
		virtual bool ShadowHit(const Ray& ray, float tmin, float tmax, float time) const = 0;
		virtual BBox BoundingBox() const = 0;
	};
}

#endif
