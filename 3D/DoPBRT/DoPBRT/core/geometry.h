/********************************************************************
	created:	2016/05/26
	created:	26:5:2016   16:35
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\geometry.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	geometry
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
		1. introduce vector in my lib
	
	purpose:	
*********************************************************************/
#ifndef __CORE_GEOMETRY_H__
#define __CORE_GEOMETRY_H__

#include "pbrt.h"

namespace dopbrt
{
	class Ray
	{
	public:
		Ray() : mint_(0.0f), maxt_(FLT_MAX), time_(0.0f), depth_(0) {}
		Ray(const Point& origin, const Vector3f& direction,
			float mint, float maxt = FLT_MAX, float time = 0.0f, int depth =0)
			: origin_(origin)
			, direction_(direction)
			, mint_(mint)
			, maxt_(maxt)
			, time_(time)
			, depth_(depth)
		{}
		Ray(const Point& origin, const Vector3f& direction, const Ray& parent,
			float mint, float maxt = FLT_MAX)
			: origin_(origin)
			, direction_(direction)
			, mint_(mint)
			, maxt_(maxt_)
			, time_(parent.time_)
			, depth_(parent.depth_ + 1)
		{}

		Point operator()(float t) const { return origin_ + direction_ * t; }

		bool HasNaNs() const
		{
			return origin_.HasNaNs() || direction_.HasNaNs() || isnan(mint_) || isnan(maxt_);
		}

		// r(t) = origin_ + t * direction_, t >= 0
		Point origin_;
		Vector3f direction_;

		// restrict ray to [r(mint), r(maxt)]
		mutable float mint_;
		mutable float maxt_;
		
		// for simulate motion blur
		float time_;
		
		// allow how many times light has bounced along current path
		int depth_;
	};

	class RayDifferential : public Ray
	{
	public:
		RayDifferential() : hasDifferentials_(false) {}
		RayDifferential(const Point& origin, const Vector3f& direction,
			float mint, float maxt = FLT_MAX, float time = 0.0f, int depth = 0)
			: Ray(origin, direction, mint, maxt, time, depth)
			, hasDifferentials_(false)
		{}
		RayDifferential(const Point& origin, const Vector3f& direction, const Ray& parent,
			float mint, float maxt = FLT_MAX)
			: Ray(origin, direction, parent, mint, maxt)
			, hasDifferentials_(false)
		{}
		explicit RayDifferential(const Ray& ray)
			: Ray(ray)
			, hasDifferentials_(false)
		{}

		bool HasNaNs() const
		{
			return Ray::HasNaNs() || (hasDifferentials_ && rxOrigin_.HasNaNs()
				|| ryOrigin_.HasNaNs() || rxDirection_.HasNaNs() || ryDirection_.HasNaNs());
		}

		void ScaleDifferentials(float s)
		{
			rxOrigin_ = origin_ + (rxOrigin_ - origin_) * s;
			ryOrigin_ = origin_ + (ryOrigin_ - origin_) * s;
			rxDirection_ = direction_ + (rxDirection_ - direction_) * s;
			ryDirection_ = direction_ + (ryDirection_ - direction_) * s;
		}

		bool hasDifferentials_;
		Point rxOrigin_;
		Point ryOrigin_;
		Vector3f rxDirection_;
		Vector3f ryDirection_;
	};
}

#endif
