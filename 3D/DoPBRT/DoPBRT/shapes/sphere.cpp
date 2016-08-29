/********************************************************************
	created:	2016/06/20
	created:	20:6:2016   17:31
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\shapes\sphere.cpp
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\shapes
	file base:	sphere
	file ext:	cpp
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#include "sphere.h"

namespace dopbrt
{
	Sphere::Sphere(const Transform* objectToWorld, const Transform* worldToObject, bool reverseOrientation, 
		float radius, float z0, float z1, float maxPhi)
		: Shape(objectToWorld, worldToObject, reverseOrientation)
		, radius_(radius)
	{
		zmin_ = clamp_t(MIN(z0, z1), -radius, radius);
		zmax_ = clamp_t(MAX(z0, z1), -radius, radius);
		thetaMin_ = acosf(clamp_t(zmin_ / radius_, -1.0f, 1.0f));
		thetaMax_ = acosf(clamp_t(zmax_ / radius_, -1.0f, 1.0f));
		phiMax_ = angle2radian(clamp_t(maxPhi, 0.0f, 360.0f));
	}

	BBox Sphere::ObjectBound() const
	{
		return BBox(Point(-radius_, -radius_, zmin_), Point(radius_, radius_, zmax_));
	}

	bool Sphere::Intersect(const Ray& ray, float& tHit, float& rayEpsilon, DifferentialGeometry& dg) const
	{
		float phi;

		// 0. transform ray to object space
		Ray rayt;
		worldToObject_->ApplyRay(ray, rayt);

		// 1. compute quadratic sphere coefficients	
		// since spherical equation: x^2 + y^2 + z^2 = r^2, substituting the parametric
		// representation of ray: r(t) = o + t * d, we have: 
		// (o(x) + t * d(x))^2 + (o(y) + t * d(y))^2 + (o(z) + t * d(z))^2 = r^2
		// we can expand this equation and gather coefficients for t: A * t^2 + B * t + C = 0
		// where: A = d(x)^2 + d(y)^2 + d(z)^2, B = 2 * (d(x) * o(x) + d(y) * o(y) + d(z) * o(z)), C = o(x)^2 + o(y)^2 + o(z)^2 - r^2
		const auto& o = rayt.origin_;
		const auto& d = rayt.direction_;
		float A = d.x * d.x + d.y * d.y + d.z * d.z;
		float B = 2 * (d.x * o.x + d.y * o.y + d.z * o.z);
		float C = o.x * o.x + o.y * o.y + o.z * o.z - radius_ * radius_;

		// 2. solve quadratic equation for t values
		// two possible solutions to this quadratic equation where ray intersects the sphere:
		// t0 = (-B - sqrt(B^2 - 4AC)) / (2A)
		// t1 = (-B + sqrt(B^2 - 4AC)) / (2A)
		float t0;
		float t1;
		if (!Quadratic(A, B, C, &t0, &t1))
			return false;

		// 3. compute intersection distance along ray
		if (t0 > rayt.maxt_ || t1 < rayt.mint_)
			return false;
		float thit = t0;
		if (t0 < rayt.mint_)
		{
			thit = t1;
			if (thit > rayt.maxt_)
				return false;
		}

		// 4. compute sphere hit position and phi
		// y / x = r*sin(theta)*sin(phi) / (r*sin(theta)*cos(phi))
		// so phi = arctan(y/x), and should remap to[0,2PI] to match sphere's original definition
		Point phit = rayt(thit);
		if (phit.x == 0.0f && phit.y == 0.0f)
			phit.x = 1e-5f * radius_; // why?
		phi = atan2f(phit.y, phit.x);
		if (phi < 0.0f)
			phi += 2.0f * M_PI;

		// 5. test sphere intersection against clipping parameters
		// first test t0, if it's invalid then test t1
		if ((zmin_ > -radius_ && phit.z < zmin_) ||
			(zmax_ <  radius_ && phit.z > zmax_) || phi > phiMax_)
		{
			if (thit == t1)
				return false;
			if (t1 > rayt.maxt_)
				return false;
			thit = t1;
			// compute sphere hit position and phi
			phit = rayt(thit);
			if (phit.x == 0.0f && phit.y == 0.0f)
				phit.x = 1e-5f * radius_; // why?
			phi = atan2f(phit.y, phit.x);
			if (phi < 0.0f)
				phi += 2.0f * M_PI;
			if ((zmin_ > -radius_ && phit.z < zmin_) ||
				(zmax_ <  radius_ && phit.z > zmax_) || phi > phiMax_)
				return false;
		}

		// 6. find parametric representation of sphere hit
		// z = r * cos(theta), so theta = acof(z/r)
		// phi = u * phiMax, u = [0, 1]
		// theta = thetaMin + v * (thetaMax - thetaMin), v = [0, 1]
		float u = phi / phiMax_;
		float theta = acosf(clamp_t(phit.z / radius_, -1.0f, 1.0f));
		float v = (theta - thetaMin_) / (thetaMax_ - thetaMin_);

		// 7. compute sphere dp/du and dp/dv

		// 8. compute sphere dn/du and dn/dv

		// 9. initialize differential geometry from parametric information

		// 10. update tHit for quadratic intersection
		tHit = thit;

		// 11. compute rayEpsilon for quadratic intersection
		rayEpsilon = 5e-4f * tHit; // why?
		return true;
	}

	bool Sphere::IntersectP(const Ray& ray) const
	{
		float phi;

		// 0. transform ray to object space
		Ray rayt;
		worldToObject_->ApplyRay(ray, rayt);

		// 1. compute quadratic sphere coefficients	
		// since spherical equation: x^2 + y^2 + z^2 = r^2, substituting the parametric
		// representation of ray: r(t) = o + t * d, we have: 
		// (o(x) + t * d(x))^2 + (o(y) + t * d(y))^2 + (o(z) + t * d(z))^2 = r^2
		// we can expand this equation and gather coefficients for t: A * t^2 + B * t + C = 0
		// where: A = d(x)^2 + d(y)^2 + d(z)^2, B = 2 * (d(x) * o(x) + d(y) * o(y) + d(z) * o(z)), C = o(x)^2 + o(y)^2 + o(z)^2 - r^2
		const auto& o = rayt.origin_;
		const auto& d = rayt.direction_;
		float A = d.x * d.x + d.y * d.y + d.z * d.z;
		float B = 2 * (d.x * o.x + d.y * o.y + d.z * o.z);
		float C = o.x * o.x + o.y * o.y + o.z * o.z - radius_ * radius_;

		// 2. solve quadratic equation for t values
		// two possible solutions to this quadratic equation where ray intersects the sphere:
		// t0 = (-B - sqrt(B^2 - 4AC)) / (2A)
		// t1 = (-B + sqrt(B^2 - 4AC)) / (2A)
		float t0;
		float t1;
		if (!Quadratic(A, B, C, &t0, &t1))
			return false;

		// 3. compute intersection distance along ray
		if (t0 > rayt.maxt_ || t1 < rayt.mint_)
			return false;
		float thit = t0;
		if (t0 < rayt.mint_)
		{
			thit = t1;
			if (thit > rayt.maxt_)
				return false;
		}

		// 4. compute sphere hit position and phi
		// y / x = r*sin(theta)*sin(phi) / (r*sin(theta)*cos(phi))
		// so phi = arctan(y/x), and should remap to[0,2PI] to match sphere's original definition
		Point phit = rayt(thit);
		if (phit.x == 0.0f && phit.y == 0.0f)
			phit.x = 1e-5f * radius_; // why?
		phi = atan2f(phit.y, phit.x);
		if (phi < 0.0f)
			phi += 2.0f * M_PI;

		// 5. test sphere intersection against clipping parameters
		// first test t0, if it's invalid then test t1
		if ((zmin_ > -radius_ && phit.z < zmin_) ||
			(zmax_ <  radius_ && phit.z > zmax_) || phi > phiMax_)
		{
			if (thit == t1)
				return false;
			if (t1 > rayt.maxt_)
				return false;
			thit = t1;
			// compute sphere hit position and phi
			phit = rayt(thit);
			if (phit.x == 0.0f && phit.y == 0.0f)
				phit.x = 1e-5f * radius_; // why?
			phi = atan2f(phit.y, phit.x);
			if (phi < 0.0f)
				phi += 2.0f * M_PI;
			if ((zmin_ > -radius_ && phit.z < zmin_) ||
				(zmax_ <  radius_ && phit.z > zmax_) || phi > phiMax_)
				return false;
		}

		return true;
	}

	float Sphere::Area() const
	{
		return phiMax_ * radius_ * (zmax_ - zmin_);
	}
}