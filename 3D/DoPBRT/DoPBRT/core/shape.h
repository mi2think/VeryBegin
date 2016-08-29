/********************************************************************
	created:	2016/06/16
	created:	16:6:2016   17:05
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\shape.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	shape
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __CORE_SHAPE_H__
#define __CORE_SHAPE_H__

#include "pbrt.h"
#include "memory.h"
#include "transform.h"
#include "diffgeom.h"

namespace dopbrt
{
	class Shape : public RefCounted
	{
	public:
		Shape(const Transform* objectToWorld, const Transform* worldToObject, bool reverseOrientation);
		virtual ~Shape();

		virtual BBox ObjectBound() = 0;
		virtual BBox WorldBound() const;
		virtual bool CanIntersect() const;
		virtual void Refine(vector<Ref<Shape>>& refined) const;
		virtual bool Intersect(const Ray& ray, float& tHit, float& rayEpsilon, DifferentialGeometry& dg) const;
		virtual bool IntersectP(const Ray& ray) const;
		virtual void GetShadingGeometry(const Transform& objToWorld, const DifferentialGeometry& dg, DifferentialGeometry& dgShading) const;
		virtual float Area() const;

		const Transform* objectToWorld_;
		const Transform* worldToObject_;
		// whether surface normal direction should reverse 
		const bool reverseOrientation_;
		const bool transformSwapsHandedness_;

		const uint32_t shapeId_;
		static uint32_t nextShapeId_;
	};
}


#endif
