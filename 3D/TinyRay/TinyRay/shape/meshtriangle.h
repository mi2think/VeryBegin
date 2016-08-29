/********************************************************************
	created:	2016/08/05
	created:	5:8:2016   22:53
	filename: 	D:\Code\TinyRay\TinyRay\shape\meshtriangle.h
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	meshtriangle
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	
*********************************************************************/
#ifndef __MESH_TRIANGLE_H__
#define __MESH_TRIANGLE_H__

#include "shape.h"

namespace tinyray
{
	class Mesh;
	class Texture;

	class MeshTriangleUV : public Shape
	{
	public:
		MeshTriangleUV();
		MeshTriangleUV(Mesh* mesh, int p0, int p1, int p2);
		~MeshTriangleUV();

		bool Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const;
		bool ShadowHit(const Ray& ray, float tmin, float tmax, float time) const;
		BBox BoundingBox() const;

		int p_[3];
		Mesh* mesh_;
	};
}

#endif
