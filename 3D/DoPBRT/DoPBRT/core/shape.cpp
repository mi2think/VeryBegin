/********************************************************************
	created:	2016/06/16
	created:	16:6:2016   17:09
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\shape.cpp
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	shape
	file ext:	cpp
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#include "shape.h"

namespace dopbrt
{
	uint32_t Shape::nextShapeId_ = 1;

	Shape::Shape(const Transform* objectToWorld, const Transform* worldToObject, bool reverseOrientation)
		: objectToWorld_(objectToWorld)
		, worldToObject_(worldToObject)
		, reverseOrientation_(reverseOrientation)
		, transformSwapsHandedness_(objectToWorld->SwapsHandedness())
		, shapeId_(nextShapeId_++)
	{

	}

	Shape::~Shape()
	{
	}

	BBox Shape::WorldBound() const
	{
		return objectToWorld_->ApplyBBox(ObjectBound());
	}

	bool Shape::CanIntersect() const
	{
		return true;
	}

	void Shape::Refine(vector<Ref<Shape>>& refined) const
	{
		LOG_FATAL("Unimplemented Shape::Refine() method called");
	}

	bool Shape::Intersect(const Ray& ray, float& tHit, float& rayEpsilon, DifferentialGeometry& dg) const
	{
		LOG_FATAL("Unimplemented Shape::Intersect() method called");
		return false;
	}

	bool Shape::IntersectP(const Ray& ray) const
	{
		LOG_FATAL("Unimplemented Shape::IntersectP() method called");
		return false;
	}

	void Shape::GetShadingGeometry(const Transform& objToWorld, const DifferentialGeometry& dg, DifferentialGeometry& dgShading) const
	{
		dgShading = dg;
	}

	float Shape::Area() const
	{
		LOG_FATAL("Unimplemented Shape::Area() method called");
		return 0;
	}
}