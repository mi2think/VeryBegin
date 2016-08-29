/********************************************************************
	created:	2016/08/05
	created:	5:8:2016   22:35
	filename: 	D:\Code\TinyRay\TinyRay\core\mesh.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	mesh
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	Mesh
*********************************************************************/
#ifndef __MESH_H__
#define __MESH_H__

#include "vertex.h"
#include "texture.h"

namespace tinyray
{
	class Mesh
	{
	public:
		Mesh() {}
		Texture* GetTexture() const { return texture_; }

		const VertexUV& GetVertexUV(int index) const { return vertexUVs_[index]; }
	private:
		Texture*	texture_;
		Vector3f*	vertexs_;
		VertexUV*	vertexUVs_;
		VertexN*	vertexNs_;
		VertexUVN*	vertexUVNS_;
	};
}

#endif