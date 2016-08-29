/********************************************************************
	created:	2016/08/05
	created:	5:8:2016   22:18
	filename: 	D:\Code\TinyRay\TinyRay\core\vertex.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	vertex
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	Vertex
*********************************************************************/
#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "math/vector2.h"
#include "math/vector3.h"

namespace tinyray
{
	struct VertexUV
	{
		Vector3f pt;
		Vector2f uv;
	};

	struct VertexN
	{
		Vector3f pt;
		Vector3f n;
	};

	struct VertexUVN
	{
		Vector3f pt;
		Vector3f n;
		Vector2f uv;
	};
}

#endif