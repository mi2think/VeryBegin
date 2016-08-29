/********************************************************************
	created:	2016/08/15
	created:	15:8:2016   17:19
	filename: 	D:\Code\TinyRay\TinyRay\shape\bvh.cpp
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	bvh
	file ext:	cpp
	copyright:
	The source come from book "Realistic Ray Tracing".

	modify:

	purpose:	BVH
*********************************************************************/
#include "bvh.h"

namespace tinyray
{
	int qsplit(Shape** shapes, int shapeNum, float pivotVal, int axis)
	{
		BBox bbox;
		float centroid = 0.0f;
		int retVal = 0;

		for (int i = 0; i < shapeNum; ++i)
		{
			bbox = shapes[i]->BoundingBox();
			centroid = (bbox.pMin_[axis] + bbox.pMax_[axis]) / 2.0f;
			if (centroid < pivotVal)
			{
				swap_t(shapes[i], shapes[retVal]);
				++retVal;
			}
		}

		if (retVal == 0 || retVal == shapeNum)
			retVal = shapeNum / 2;
		return retVal;
	}

	BVH::BVH()
		: shapeL_(nullptr)
		, shapeR_(nullptr)
	{}

	BVH::BVH(Shape** shapes, int shapeNum)
	{
		if (shapeNum == 1) *this = BVH(shapes[0], shapes[0]);
		if (shapeNum == 2) *this = BVH(shapes[0], shapes[1]);

		// find midpoint of bounding box to use as pivot
		for (int i = 0; i < shapeNum; ++i)
		{
			bbox_ |= shapes[i]->BoundingBox();
		}
		Vector3f pivot = bbox_.Center();	
		int midPoint = qsplit(shapes, shapeNum, pivot.x, 0);

		// create a new bounding volume
		shapeL_ = BuildBranch(shapes, midPoint, 1);
		shapeR_ = BuildBranch(&shapes[midPoint], shapeNum - midPoint, 1);
	}

	BVH::BVH(Shape* shapeL, Shape* shapeR)
		: shapeL_(shapeL)
		, shapeR_(shapeR)
	{
		bbox_ |= shapeL_->BoundingBox();
		bbox_ |= shapeR_->BoundingBox();
	}

	BVH::BVH(Shape* shapeL, Shape* shapeR, const BBox& bbox)
		: shapeL_(shapeL)
		, shapeR_(shapeR)
		, bbox_(bbox)
	{
	}

	bool BVH::Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const
	{
		if (bbox_.Intersect(ray, tmin, tmax))
			return false;

		record.t = tmax;
		// call hit on both branches
		return shapeL_->Hit(ray, tmin, tmax, time, record)
			|| shapeR_->Hit(ray, tmin, record.t, time, record);
	}

	bool BVH::ShadowHit(const Ray& ray, float tmin, float tmax, float time) const
	{
		if (bbox_.Intersect(ray, tmin, tmax))
			return false;

		return shapeL_->ShadowHit(ray, tmin, tmax, time)
			|| shapeR_->ShadowHit(ray, tmin, tmax, time);
	}

	BBox BVH::BoundingBox() const
	{
		return bbox_;
	}

	Shape* BVH::BuildBranch(Shape** shapes, int shapeNum, int axis)
	{
		if (shapeNum == 1) return shapes[0];
		if (shapeNum == 2) return new BVH(shapes[0], shapes[1]);

		// find midpoint of bounding box to use as pivot
		BBox bbox;
		for (int i = 0; i < shapeNum; ++i)
		{
			bbox |= shapes[i]->BoundingBox();
		}
		Vector3f pivot = bbox.Center();
		int midPoint = qsplit(shapes, shapeNum, pivot[axis], axis);

		// create a new bounding volume
		Shape* shapeL = BuildBranch(shapes, midPoint, (axis + 1) % 3);
		Shape* shapeR = BuildBranch(&shapes[midPoint], shapeNum - midPoint, (axis + 1) % 3);
		return new BVH(shapeL, shapeR);
	}
}