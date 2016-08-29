/********************************************************************
	created:	2016/06/16
	created:	16:6:2016   10:51
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\diffgeom.cpp
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	diffgeom
	file ext:	cpp
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#include "diffgeom.h"

namespace dopbrt
{
	DifferentialGeometry::DifferentialGeometry()
		: shape_(nullptr)
		, u_(0)
		, v_(0)
		, dudx_(0)
		, dudy_(0)
		, dvdx_(0)
		, dvdy_(0)
	{}

	DifferentialGeometry::DifferentialGeometry(const Shape* shape, const Point& p, float u, float v, 
		const Vector3f& dpdu, const Vector3f& dpdv, const Normal& dndu, const Normal& dndv)
		: p_(p)
		, u_(u)
		, v_(v)
		, dpdu_(dpdu)
		, dpdv_(dpdv)
		, dndu_(dndu)
		, dndv_(dndv)
		, shape_(shape)
		, dudx_(0)
		, dudy_(0)
		, dvdx_(0)
		, dvdy_(0)
	{
		Vector3f n = CrossProduct(dpdu_, dpdv_);
		n.Normalize();
		n_ = n;

		// adjust normal based on orientation and handedness
		if (shape && (shape_->reverseOrientation_ ^ shape_->transformSwapsHandedness_))
			n_ *= -1.0f;
	}
}
