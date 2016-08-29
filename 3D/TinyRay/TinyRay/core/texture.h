/********************************************************************
	created:	2016/08/03
	created:	3:8:2016   14:53
	filename: 	D:\Code\TinyRay\TinyRay\core\texture.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	texture
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".

	modify:

	purpose:	Texture
*********************************************************************/
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "core/rgb.h"
#include "math/vector2.h"
#include "math/vector3.h"

namespace tinyray
{
	class Texture
	{
	public:
		virtual ~Texture() {}

		virtual RGB Value(const Vector2f& uv, const Vector3f& pt) const = 0;
	};
}

#endif
