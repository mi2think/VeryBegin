/********************************************************************
	created:	2016/07/07
	created:	7:7:2016   22:48
	filename: 	D:\Code\TinyRay\TinyRay\core\ray.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	ray
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Ray
*********************************************************************/
#ifndef __RAY_H__
#define __RAY_H__

#include "math/vector3.h"

namespace tinyray
{
	// r(t) = origin_ + t * direction_, t >= 0
	class Ray
	{
	public:
		Vector3f origin_;
		Vector3f direction_;

		Ray() {}
		Ray(const Vector3f& org, const Vector3f& dir)
			: origin_(org)
			, direction_(dir) 
		{}

		Vector3f PointAt(float t)  const { return origin_ + direction_ * t; }
	};
}

#endif
