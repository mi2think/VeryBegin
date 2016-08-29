/********************************************************************
	created:	2016/06/20
	created:	20:6:2016   17:18
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\shapes\sphere.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\shapes
	file base:	sphere
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __SHAPES_SPHERE_H__
#define __SHAPES_SPHERE_H__

#include "../core/shape.h"

namespace dopbrt
{
	// a sphere centered at the origin in object space 
	// spherical coordinate:
	// x = r*sin(theta)*cos(phi)
	// y = r*sin(theta)*sin(phi)
	// z = r*cos(theta)
	class Sphere : public Shape
	{
	public:
		Sphere(const Transform* objectToWorld, const Transform* worldToObject, bool reverseOrientation,
			float radius, float z0, float z1, float maxPhi);

		BBox ObjectBound() const;
		bool Intersect(const Ray& ray, float& tHit, float& rayEpsilon, DifferentialGeometry& dg) const override;
		bool IntersectP(const Ray& ray) const override;
		float Area() const override;
	private:
		float radius_;
		float phiMax_;
		float zmin_;
		float zmax_;
		float thetaMin_;
		float thetaMax_;
	};
}

#endif
