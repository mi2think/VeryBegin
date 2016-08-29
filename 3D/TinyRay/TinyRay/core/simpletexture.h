/********************************************************************
	created:	2016/08/03
	created:	3:8:2016   15:03
	filename: 	D:\Code\TinyRay\TinyRay\core\simpletexture.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	simpletexture
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".

	modify:

	purpose:	Simple Texture
*********************************************************************/
#ifndef __SIMPLE_TEXTURE_H__
#define __SIMPLE_TEXTURE_H__

#include "texture.h"

namespace tinyray
{
	class SimpleTexture : public Texture
	{
	public:
		SimpleTexture(const RGB& c) : color_(c) {}

		RGB Value(const Vector2f& uv, const Vector3f& pt) const { return color_; }
	private:
		RGB color_;
	};
}

#endif
