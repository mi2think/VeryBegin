/********************************************************************
	created:	2016/07/08
	created:	8:7:2016   23:47
	filename: 	D:\Code\TinyRay\TinyRay\shape\triangle.h
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	triangle
	file ext:	cpp
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:
		Hit using "Fast Minimum Storage Ray Triangle Intersection"

	purpose:	
*********************************************************************/
#include "triangle.h"
#include "math/math_fwd.h"

namespace tinyray
{
	Triangle::Triangle(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const RGB& color)
		: p0_(p0)
		, p1_(p1)
		, p2_(p2)
		, color_(color)
	{}

	bool Triangle::Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const
	{
		Vector3f e1 = p1_ - p0_;
		Vector3f e2 = p2_ - p0_;
		Vector3f q = CrossProduct(ray.direction_, e2);
		float det = DotProduct(e1, q);

		if (ABS(det) < EPSILON_E5)
			return false;

		float invdet = 1.0f / det;

		// Compute first barycentric coordinate
		Vector3f s = ray.origin_ - p0_;
		float u = DotProduct(s, q) * invdet;
		if (u < 0.0f || u > 1.0f)
			return false;

		// Compute second barycentric coordinate
		Vector3f r = CrossProduct(s, e1);
		float v = DotProduct(ray.direction_, r) * invdet;
		if (v < 0.0f || u + v > 1.0f)
			return false;

		float t = DotProduct(e2, r) * invdet;
		if (t < tmin || t > tmax)
			return false;

		record.normal = CrossProduct(e1, e2);
		record.normal.Normalize();
		record.t = t;
		record.color = color_;
		return true;
	}

	bool Triangle::ShadowHit(const Ray& ray, float tmin, float tmax, float time) const
	{
		Vector3f e1 = p1_ - p0_;
		Vector3f e2 = p2_ - p0_;
		Vector3f q = CrossProduct(ray.direction_, e2);
		float det = DotProduct(e1, q);

		if (ABS(det) < EPSILON_E5)
			return false;

		float invdet = 1.0f / det;

		// Compute first barycentric coordinate
		Vector3f s = ray.origin_ - p0_;
		float u = DotProduct(s, q) * invdet;
		if (u < 0.0f || u > 1.0f)
			return false;

		// Compute second barycentric coordinate
		Vector3f r = CrossProduct(s, e1);
		float v = DotProduct(ray.direction_, r) * invdet;
		if (v < 0.0f || u + v > 1.0f)
			return false;

		float t = DotProduct(e2, r) * invdet;
		return t >= tmin && t < tmax;
	}

	BBox Triangle::BoundingBox() const
	{
		BBox bbox;
		bbox.Union(p0_);
		bbox.Union(p1_);
		bbox.Union(p2_);
		return bbox;
	}
}