/********************************************************************
	created:	2016/07/09
	created:	9:7:2016   2:29
	filename: 	D:\Code\TinyRay\TinyRay\shape\sphere.h
	filename: 	D:\Code\TinyRay\TinyRay\shape
	file base:	sphere
	file ext:	cpp
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	
*********************************************************************/
#include "sphere.h"
#include "math/math_fwd.h"

namespace tinyray
{
	Sphere::Sphere(const Vector3f& center, float radius, const RGB& color)
		: center_(center)
		, radius_(radius)
		, color_(color)
	{}

	bool Sphere::Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const
	{
		// compute quadratic sphere coefficients
		const auto& o = ray.origin_;
		const auto& d = ray.direction_;
		Vector3f temp = o - center_;
		float A = DotProduct(d, d);
		float B = 2 * DotProduct(d, temp);
		float C = DotProduct(temp, temp) - radius_ * radius_;

		// solve quadratic equation for t values
		float t0;
		float t1;
		if (!quadratic(A, B, C, &t0, &t1))
			return false;

		// compute intersection distance along ray
		if (t0 > tmax || t1 < tmin)
			return false;

		float t = t0;
		if (t < tmin)
		{
			t = t1;
			if (t > tmax)
				return false;
		}

		record.t = t;
		record.normal = ray.origin_ + ray.direction_ * t - center_;
		record.normal.Normalize();
		record.color = color_;

		return true;
	}

	bool Sphere::ShadowHit(const Ray& ray, float tmin, float tmax, float time) const
	{
		// compute quadratic sphere coefficients	
		// since spherical equation: (x - x0)^2 + (y - y0)^2 + (z - z0)^2 = r^2, substituting the parametric
		// representation of ray: r(t) = o + t * d, we have: 
		// (o(x) + t * d(x) - x0)^2 + (o(y) + t * d(y) - y0)^2 + (o(z) + t * d(z) - z0)^2 = r^2
		// we can expand this equation and gather coefficients for t: A * t^2 + B * t + C = 0
		const auto& o = ray.origin_;
		const auto& d = ray.direction_;
		Vector3f temp = o - center_;
		float A = DotProduct(d, d);
		float B = 2 * DotProduct(d, temp);
		float C = DotProduct(temp, temp) - radius_ * radius_;

		// solve quadratic equation for t values
		// two possible solutions to this quadratic equation where ray intersects the sphere:
		// t0 = (-B - sqrt(B^2 - 4AC)) / (2A)
		// t1 = (-B + sqrt(B^2 - 4AC)) / (2A)
		float t0;
		float t1;
		if (!quadratic(A, B, C, &t0, &t1))
			return false;

		// compute intersection distance along ray
		if (t0 > tmax || t1 < tmin)
			return false;

		if (t0 < tmin && t1 > tmax)
			return false;

		return true;
	}

	BBox Sphere::BoundingBox() const
	{
		return BBox(center_ - Vector3f(radius_, radius_, radius_),
			center_ + Vector3f(radius_, radius_, radius_));
	}
}
