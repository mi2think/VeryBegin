/********************************************************************
	created:	2016/06/14
	created:	14:6:2016   17:33
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\diffgeom.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	diffgeom
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __CORE_DIFFGEOM_H__
#define __CORE_DIFFGEOM_H__

#include "pbrt.h"

namespace dopbrt
{
	class DifferentialGeometry
	{
	public:
		DifferentialGeometry();
		DifferentialGeometry(const Shape* shape, const Point& p, float u, float v,
			const Vector3f& dpdu, const Vector3f& dpdv, const Normal& dndu, const Normal& dndv);

		// the position of 3D point p
		Point p_;
		// the surface normal n at the point
		Normal n_;
		// u,v coordinates from parameterization of the surface
		float u_;
		float v_;
		// the parametric partial derivatives dp/du and dp/dv
		Vector3f dpdu_;
		Vector3f dpdv_;
		// the partial derivatives of change in surface normal dn/du and dn/dv
		Normal dndu_;
		Normal dndv_;

		const Shape* shape_;

		mutable Vector3f dpdx_;
		mutable Vector3f dpdy_;
		mutable float dudx_;
		mutable float dudy_;
		mutable float dvdx_;
		mutable float dvdy_;
	};
}

#endif