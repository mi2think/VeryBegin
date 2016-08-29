/********************************************************************
	created:	2016/08/04
	created:	4:8:2016   20:33
	filename: 	D:\Code\TinyRay\TinyRay\core\imagetexture.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	imagetexture
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	Image Texture
*********************************************************************/
#ifndef __IMAGE_TEXTURE_H__
#define __IMAGE_TEXTURE_H__

#include "texture.h"

namespace tinyray
{
	class ImageTexture : public Texture
	{
	public:
		ImageTexture(const char* fileName)
		{
			image_.ReadPPM(fileName);
			width_ = image_.GetWidth();
			height_ = image_.GetHeight();
		}

		RGB Value(const Vector2f& uv, const Vector3f& pt) const
		{
			//  4 texels
			//  |  0  |  1  |
			//  |-----|-----|
			//  |  3  |  2  |

			int u = (int)(uv.x * width_);
			int v = (int)(uv.y * height_);

			const RGB* pixel = &(image_.GetRaster()[0]);
			const RGB* p0 = pixel + v * width_ + u * height_;
			const RGB* p3 = p0;
			if (v + 1 < height_)
				p3 += width_;
			const RGB* p1 = p0;
			const RGB* p2 = p3;
			if (u + 1 < width_)
			{
				++p1;
				++p2;
			}

			float du = uv.x - (int)uv.x;
			float dv = uv.y - (int)uv.y;
			float one_minus_du = 1.0f - du;
			float one_minus_dv = 1.0f - dv;

			RGB c;
			c += *p0 * (one_minus_du * one_minus_dv);
			c += *p1 * (du * one_minus_dv);
			c += *p2 * (du * dv);
			c += *p3 * (dv * one_minus_du);
			return c;
		}

	private:
		PPM image_;
		int width_;
		int height_;
	};
}

#endif