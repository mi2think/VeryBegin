/********************************************************************
	created:	2016/08/08
	created:	8:8:2016   18:04
	filename: 	D:\Code\TinyRay\TinyRay\shape\instance.h
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	instance
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".

	modify:

	purpose:	instance of shape
*********************************************************************/
#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include "shape.h"
#include "math/matrix44.h"

namespace tinyray
{
	class Instance : public Shape
	{
	public:
		Instance();
		Instance(const Matrix44f& trans, const Matrix44f& transInv, Shape* shape);
		Instance(const Matrix44f& trans, Shape* shape);
		~Instance();

		bool Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const;
		bool ShadowHit(const Ray& ray, float tmin, float tmax, float time) const;
		BBox BoundingBox() const;

		Matrix44f trans_;
		Matrix44f transInv_;
		Shape* shape_;
	};
}

#endif