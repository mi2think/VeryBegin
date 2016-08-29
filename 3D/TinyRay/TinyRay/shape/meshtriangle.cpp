/********************************************************************
	created:	2016/08/05
	created:	5:8:2016   22:58
	filename: 	D:\Code\TinyRay\TinyRay\shape\meshtriangle.cpp
	file path:	D:\Code\TinyRay\TinyRay\shape
	file base:	meshtriangle
	file ext:	cpp
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	
*********************************************************************/
#include "meshtriangle.h"
#include "core/mesh.h"

namespace tinyray
{
	MeshTriangleUV::MeshTriangleUV()
		: mesh_(nullptr)
	{}

	MeshTriangleUV::MeshTriangleUV(Mesh* mesh, int p0, int p1, int p2)
		: mesh_(mesh)
	{
		p_[0] = p0;
		p_[1] = p1;
		p_[2] = p2;
	}

	MeshTriangleUV::~MeshTriangleUV()
	{}

	bool MeshTriangleUV::Hit(const Ray& ray, float tmin, float tmax, float time, HitRecord& record) const
	{
		const auto& p0 = mesh_->GetVertexUV(p_[0]).pt;
		const auto& p1 = mesh_->GetVertexUV(p_[1]).pt;
		const auto& p2 = mesh_->GetVertexUV(p_[2]).pt;

		Vector3f e1 = p1 - p0;
		Vector3f e2 = p2 - p0;
		Vector3f q = CrossProduct(ray.direction_, e2);
		float det = DotProduct(e1, q);

		if (ABS(det) < EPSILON_E5)
			return false;

		float invdet = 1.0f / det;

		// Compute first barycentric coordinate
		Vector3f s = ray.origin_ - p0;
		float u = DotProduct(s, q) * invdet;
		if (u < 0.0f || u > 1.0f)
			return false;

		// Compute second barycentric coordinate
		Vector3f r = CrossProduct(s, e1);
		float v = DotProduct(ray.direction_, r) * invdet;
		if (v < 0.0f || u + v > 1.0f)
			return false;

		float t = DotProduct(e2, r) * invdet;
		if (t < tmin || t > tmax)
			return false;

		float w = 1.0f - u - v;

		const auto& uv0 = mesh_->GetVertexUV(p_[0]).uv;
		const auto& uv1 = mesh_->GetVertexUV(p_[1]).uv;
		const auto& uv2 = mesh_->GetVertexUV(p_[2]).uv;

		record.normal = CrossProduct(e1, e2);
		record.normal.Normalize();
		record.t = t;
		record.tex = mesh_->GetTexture();
		record.uv = w * uv0 + u * uv1 + v * uv2;

		return true;
	}

	bool MeshTriangleUV::ShadowHit(const Ray& ray, float tmin, float tmax, float time) const
	{
		const auto& p0 = mesh_->GetVertexUV(p_[0]).pt;
		const auto& p1 = mesh_->GetVertexUV(p_[1]).pt;
		const auto& p2 = mesh_->GetVertexUV(p_[2]).pt;

		Vector3f e1 = p1 - p0;
		Vector3f e2 = p2 - p0;
		Vector3f q = CrossProduct(ray.direction_, e2);
		float det = DotProduct(e1, q);

		if (ABS(det) < EPSILON_E5)
			return false;

		float invdet = 1.0f / det;

		// Compute first barycentric coordinate
		Vector3f s = ray.origin_ - p0;
		float u = DotProduct(s, q) * invdet;
		if (u < 0.0f || u > 1.0f)
			return false;

		// Compute second barycentric coordinate
		Vector3f r = CrossProduct(s, e1);
		float v = DotProduct(ray.direction_, r) * invdet;
		if (v < 0.0f || u + v > 1.0f)
			return false;

		float t = DotProduct(e2, r) * invdet;
		return t >= tmin && t < tmax;
	}

	BBox MeshTriangleUV::BoundingBox() const
	{
		const auto& p0 = mesh_->GetVertexUV(p_[0]).pt;
		const auto& p1 = mesh_->GetVertexUV(p_[1]).pt;
		const auto& p2 = mesh_->GetVertexUV(p_[2]).pt;

		BBox bbox;
		bbox.Union(p0);
		bbox.Union(p1);
		bbox.Union(p2);
		return bbox;
	}
}