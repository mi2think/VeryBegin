/********************************************************************
	created:	2016/08/15
	created:	15:8:2016   16:34
	filename: 	D:\Code\TinyRay\TinyRay\shape\bbox.cpp
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	bbox
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Bounding Box
*********************************************************************/
#include "bbox.h"
#include "math/vector4.h"
#include "math/matrix44.h"

namespace tinyray
{
	bool BBox::Overlaps(const BBox& bbox) const
	{
		bool x = pMax_.x >= bbox.pMin_.x && pMin_.x <= pMax_.x;
		bool y = pMax_.y >= bbox.pMin_.y && pMin_.y <= pMax_.y;
		bool z = pMax_.z >= bbox.pMin_.z && pMin_.z <= pMax_.z;
		return x && y && z;
	}

	BBox& BBox::Union(const Vector3f& pt)
	{
		if (pt.x < pMin_.x) pMin_.x = pt.x;
		if (pt.y < pMin_.y) pMin_.y = pt.y;
		if (pt.z < pMin_.z) pMin_.z = pt.z;

		if (pt.x > pMax_.x) pMax_.x = pt.x;
		if (pt.y > pMin_.y) pMax_.y = pt.y;
		if (pt.z > pMin_.z) pMax_.z = pt.z;

		return *this;
	}

	BBox& BBox::Union(const BBox& bbox)
	{
		if (&bbox != this)
		{
			if (bbox.pMin_.x < pMin_.x) pMin_.x = bbox.pMin_.x;
			if (bbox.pMin_.y < pMin_.y) pMin_.y = bbox.pMin_.y;
			if (bbox.pMin_.z < pMin_.z) pMin_.z = bbox.pMin_.z;

			if (bbox.pMax_.x > pMax_.x) pMax_.x = bbox.pMax_.x;
			if (bbox.pMax_.y > pMax_.y) pMax_.y = bbox.pMax_.y;
			if (bbox.pMax_.z > pMax_.z) pMax_.z = bbox.pMax_.z;
		}

		return *this;
	}

	BBox& BBox::Transform(const Matrix44f& m)
	{
		Reset();

		Vector4f pts[] = {
			{ pMin_.x, pMin_.y, pMin_.z, 1 },
			{ pMax_.x, pMin_.y, pMin_.z, 1 },
			{ pMin_.x, pMax_.y, pMin_.z, 1 },
			{ pMin_.x, pMin_.y, pMax_.z, 1 },
			{ pMin_.x, pMax_.y, pMax_.z, 1 },
			{ pMax_.x, pMax_.y, pMin_.z, 1 },
			{ pMax_.x, pMin_.y, pMax_.z, 1 },
			{ pMax_.x, pMax_.y, pMax_.z, 1 }
		};

		for (auto& pt : pts)
		{
			pt *= m;
			Union(pt.DivW());
		}
		return *this;
	}

	bool BBox::Intersect(const Ray& ray, float tmin, float tmax) const
	{
		float t0 = tmin;
		float t1 = tmax;
		for (int i = 0; i < 3; ++i)
		{
			float invRayDir = 1.0f / ray.direction_[i];
			float tnear = (pMin_[i] - ray.origin_[i]) * invRayDir;
			float tfar = (pMax_[i] - ray.origin_[i]) * invRayDir;

			if (tnear > tfar)
				swap_t(tnear, tfar);
			t0 = tnear > t0 ? tnear : t0;
			t1 = tfar < t1 ? tfar : t1;
			if (t0 > t1)
				return false;
		}
		return true;
	}
}