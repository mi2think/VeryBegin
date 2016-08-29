/********************************************************************
	created:	2016/08/08
	created:	8:8:2016   18:21
	filename: 	D:\Code\TinyRay\TinyRay\shape\instance.cpp
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	instance
	file ext:	cpp
	copyright:
		The source come from book "Realistic Ray Tracing".

	modify:

	purpose:	instance of shape
*********************************************************************/
#include "instance.h"

namespace tinyray
{
	Instance::Instance()
		: shape_(nullptr)
	{}

	Instance::Instance(const Matrix44f& trans, const Matrix44f& transInv, Shape* shape)
		: trans_(trans)
		, transInv_(transInv)
		, shape_(shape)
	{}

	Instance::Instance(const Matrix44f& trans, Shape* shape)
		: trans_(trans)
		, shape_(shape)
	{
		MatrixInverse(transInv_, trans_);
	}

	Instance::~Instance()
	{}

	bool Instance::Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const
	{
		// transform ray to object space
		Vector3f o = ray.origin_ * transInv_;
		Vector3f d = TransformVec3(ray.direction_, transInv_);	
		Ray r(o, d);

		if (shape_->Hit(r, tmin, tmax, time, record))
		{
			// hit position is in object space
			record.hitPos = record.hitPos * trans_;
			auto m = transInv_;
			m.Transpose();
			record.normal = TransformVec3(record.normal, m);
			return true;
		}
		return false;
	}

	bool Instance::ShadowHit(const Ray& ray, float tmin, float tmax, float time) const
	{
		// transform ray to object space
		Vector3f o = ray.origin_ * transInv_;
		Vector3f d = TransformVec3(ray.direction_, transInv_);
		Ray r(o, d);
		return shape_->ShadowHit(r, tmin, tmax, time);
	}

	BBox Instance::BoundingBox() const
	{
		BBox bbox = shape_->BoundingBox();
		bbox.Transform(trans_);
		return bbox;
	}
}