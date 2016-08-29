/********************************************************************
	created:	2016/08/15
	created:	15:8:2016   17:19
	filename: 	D:\Code\TinyRay\TinyRay\shape\bvh.h
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	bvh
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".

	modify:

	purpose:	BVH
*********************************************************************/
#ifndef __BVH_H__
#define __BVH_H__

#include "shape.h"
#include "bbox.h"

namespace tinyray
{
	class BVH : public Shape
	{
	public:
		BBox bbox_;
		// maybe shapeL_ and shapeR_ point same shape
		Shape* shapeL_;
		Shape* shapeR_;

		BVH();
		BVH(Shape** shapes, int shapeNum);
		BVH(Shape* shapeL, Shape* shapeR);
		BVH(Shape* shapeL, Shape* shapeR, const BBox& bbox);

		bool Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const;
		bool ShadowHit(const Ray& ray, float tmin, float tmax, float time) const;
		BBox BoundingBox() const;
		Shape* BuildBranch(Shape** shapes, int shapeNum, int axis = 0);
	};
}

#endif
