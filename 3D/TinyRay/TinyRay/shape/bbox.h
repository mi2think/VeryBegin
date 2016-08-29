/********************************************************************
	created:	2016/08/15
	created:	15:8:2016   16:34
	filename: 	D:\Code\TinyRay\TinyRay\shape\bbox.h
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	bbox
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Bounding Box
*********************************************************************/
#ifndef __BBOX_H__
#define __BBOX_H__

#include "core/ray.h"
#include "math/math_fwd.h"
#include "math/vector3.h"

namespace tinyray
{
	class BBox
	{
	public:
		Vector3f pMin_;
		Vector3f pMax_;

		BBox() { Reset(); }
		BBox(const Vector3f& mi, const Vector3f& ma) : pMin_(mi), pMax_(ma) {}
		BBox(const BBox& bbox) : pMin_(bbox.pMin_), pMax_(bbox.pMax_) {}
		BBox& operator=(const BBox& bbox) { pMin_ = bbox.pMin_; pMax_ = bbox.pMax_; return *this; }
		BBox& operator|=(const BBox& bbox) { return Union(bbox); }

		bool Valid() const { return pMin_.x <= pMax_.x && pMin_.y <= pMax_.y && pMin_.z <= pMax_.z; }
		void Reset() { pMin_.x = pMin_.y = pMin_.z = FLT_MAX; pMax_.x = pMax_.y = pMax_.z = FLT_MIN; }
		Vector3f Size() const { return pMax_ - pMin_; }
		Vector3f Center() const { return (pMax_ + pMin_) * 0.5f; }
		void Expand(float delta) { Vector3f v(delta, delta, delta); pMin_ -= v; pMax_ += v; }
		bool Inside(const Vector3f& pt) const
		{
			return pt.x >= pMin_.x && pt.x <= pMax_.x
				&& pt.y >= pMin_.y && pt.y <= pMax_.y
				&& pt.z >= pMin_.z && pt.z <= pMax_.z;
		}
		bool Overlaps(const BBox& bbox) const;

		float SurfaceArea() const
		{
			Vector3f size = Size();
			return 2.f * (size.x * size.y + size.x * size.z + size.y * size.z);
		}

		float Volume() const
		{
			Vector3f size = Size();
			return size.x * size.y * size.z;
		}

		void BoundingSphere(Vector3f& c, float& radius) const
		{
			c = Center();
			radius = Inside(c) ? Distance(c, pMax_) : 0.f;
		}

		BBox& Union(const Vector3f& pt);
		BBox& Union(const BBox& bbox);
		BBox& Transform(const Matrix44f& m);
		bool Intersect(const Ray& ray, float tmin, float tmax) const;
	};
}

#endif
