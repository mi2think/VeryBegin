/********************************************************************
	created:	2016/08/04
	created:	4:8:2016   21:26
	filename: 	D:\Code\TinyRay\TinyRay\shape\uvsphere.cpp
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	uvsphere
	file ext:	cpp
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	
*********************************************************************/
#include "uvsphere.h"

namespace tinyray
{
	UVSphere::UVSphere(const Vector3f& center, float radius, Texture* texture)
		: center_(center)
		, radius_(radius)
		, texture_(texture)
	{}

	bool UVSphere::Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const
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

		// compute sphere hit position
		record.t = t;
		record.hitPos = ray.PointAt(t);
		record.normal = (record.hitPos - center_) / radius_;

		// compute phi and theta
		// y / x = r*sin(theta)*sin(phi) / (r*sin(theta)*cos(phi))
		// so phi = arctan(y/x), and should remap to[0,2PI] to match sphere's original definition
		// z = r * cos(theta), so theta = acof(z/r)
		Vector3f n = record.normal;
		float phi = atan2(n.y, n.x);
		if (phi < 0.0f)
			phi += 2 * M_PI;
		float theta = acos(clamp_t(n.z, -1.0f, 1.0f));

		// compute uv
		// phi = u * 2PI, u = [0, 1]; theta = v * PI, v = [0, 1]
		float one_over_pi = 1.0f / M_PI;
		float u = phi * one_over_pi * 0.5f;
		float v = theta *  one_over_pi;

		record.uv = Vector2f(u, v);
		record.tex = texture_;
		return true;
	}

	bool UVSphere::ShadowHit(const Ray& ray, float tmin, float tmax, float time) const
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

	BBox UVSphere::BoundingBox() const
	{
		return BBox(center_ - Vector3f(radius_, radius_, radius_),
			center_ + Vector3f(radius_, radius_, radius_));
	}
}