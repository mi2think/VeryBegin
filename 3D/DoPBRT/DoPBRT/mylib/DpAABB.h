/********************************************************************
	created:	2015/11/09
	created:	9:11:2015   22:34
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpAABB.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpAABB
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	AABB
*********************************************************************/
#ifndef __DP_AABB__
#define __DP_AABB__

#include "DpMath.h"
#include "DpVector3.h"
#include "DpVector4.h"
#include "DpMatrix44.h"

namespace dopixel
{
	namespace math
	{
		class AABB
		{
		public:
			Vector3f min_;
			Vector3f max_;

			AABB() { Reset(); }
			AABB(const Vector3f& mi, const Vector3f& ma) : min_(mi), max_(ma) {}
			AABB(const AABB& aabb) : min_(aabb.min_), max_(aabb.max_) {}
			AABB& operator=(const AABB& aabb) { min_ = aabb.min_; max_ = aabb.max_; return *this; }

			bool operator==(const AABB& aabb) const { return min_ == aabb.min_ && max_ == aabb.max_; }
			bool operator!=(const AABB& aabb) const { return !operator==(aabb); }

			Vector3f Size() const { return max_ - min_; }
			float XSize() const { return max_.x - min_.x; }
			float YSize() const { return max_.y - min_.y; }
			float ZSize() const { return max_.z - min_.z; }

			Vector3f Center() const { return (max_ + min_) * 0.5f; }

			bool Valid() const { return min_.x <= max_.x && min_.y <= max_.y && min_.z <= max_.z; }
			bool Overlaps(const AABB& aabb) const
			{
				bool x = max_.x >= aabb.min_.x && min_.x <= max_.x;
				bool y = max_.y >= aabb.min_.y && min_.y <= max_.y;
				bool z = max_.z >= aabb.min_.z && min_.z <= max_.z;
				return x && y && z;
			}
			void Reset()
			{
				min_.x = min_.y = min_.z = FLT_MAX;
				max_.x = max_.y = max_.z = FLT_MIN;
			}

			AABB& Union(const Vector3f& pt)
			{
				if (pt.x < min_.x) min_.x = pt.x;
				if (pt.y < min_.y) min_.y = pt.y;
				if (pt.z < min_.z) min_.z = pt.z;

				if (pt.x > max_.x) max_.x = pt.x;
				if (pt.y > min_.y) max_.y = pt.y;
				if (pt.z > min_.z) max_.z = pt.z;

				return *this;
			}
			AABB& Union(const AABB& aabb)
			{
				if (&aabb != this)
				{
					if (aabb.min_.x < min_.x) min_.x = aabb.min_.x;
					if (aabb.min_.y < min_.y) min_.y = aabb.min_.y;
					if (aabb.min_.z < min_.z) min_.z = aabb.min_.z;

					if (aabb.max_.x > max_.x) max_.x = aabb.max_.x;
					if (aabb.max_.y > max_.y) max_.y = aabb.max_.y;
					if (aabb.max_.z > max_.z) max_.z = aabb.max_.z;
				}

				return *this;
			}

			void Expand(float delta)
			{
				Vector3f v(delta, delta, delta);
				min_ -= v;
				max_ += v;
			}

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

			// which axis is longest ?
			// 0: x, 1 : y, 2: z
			int MaximumExtent() const
			{
				Vector3f size = Size();
				if (size.x > size.y && size.x > size.z)
					return 0;
				else if (size.y > size.z)
					return 1;
				else
					return 2;
			}

			Vector3f Lerp(float tx, float ty, float tz) const
			{
				return Vector3f(::Lerp(tx, min_.x, max_.x), ::Lerp(ty, min_.y, max_.y), ::Lerp(tz, min_.z, max_.z));
			}

			Vector3f Offset(const Vector3f& pt) const
			{
				Vector3f size = Size();
				return Vector3f((pt.x - min_.x) / size.x, (pt.y - min_.y) / size.y, (pt.z - min_.z) / size.z);
			}

			void BoundingSphere(Vector3f& c, float& radius) const
			{
				c = Center();
				radius = Inside(c) ? Distance(c, max_) : 0.f;
			}

			AABB& Transform(const Matrix44f& m)
			{
				Reset();

				Vector4f pts[] = {
					{min_.x, min_.y, min_.z, 1},
					{max_.x, min_.y, min_.z, 1},
					{min_.x, max_.y, min_.z, 1},
					{min_.x, min_.y, max_.z, 1},
					{min_.x, max_.y, max_.z, 1},
					{max_.x, max_.y, min_.z, 1},
					{max_.x, min_.y, max_.z, 1},
					{max_.x, max_.y, max_.z, 1}
				};

				for (auto& pt : pts)
				{
					pt *= m;
					Union(pt.DivW());
				}
				return *this;
			}

			bool Contains(const Vector3f& pt) const
			{
				return pt.x >= min_.x && pt.x <= max_.x
					&& pt.y >= min_.y && pt.y <= max_.y
					&& pt.z >= min_.z && pt.z <= max_.z;
			}

			bool Inside(const Vector3f& pt) const
			{
				return Contains(pt);
			}
		};
	}
}

#endif
