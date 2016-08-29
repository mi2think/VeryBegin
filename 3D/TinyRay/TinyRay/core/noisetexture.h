/********************************************************************
	created:	2016/08/03
	created:	3:8:2016   15:07
	filename: 	D:\Code\TinyRay\TinyRay\core\noisetexture.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	noisetexture
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".

	modify:

	purpose:	Noise Texture
*********************************************************************/
#ifndef __NOISE_TEXTURE_H__
#define __NOISE_TEXTURE_H__

#include "texture.h"
#include "perlinnoise.h"

namespace tinyray
{
	class NoiseTexture : public Texture
	{
	public:
		NoiseTexture(float frequency, const RGB& c0, const RGB& c1)
			: frequency_(frequency)
			, color0_(c0)
			, color1_(c1)
		{}

		RGB Value(const Vector2f& uv, const Vector3f& pt) const
		{
			float t = (noise_.Eval(pt * frequency_) + 1.0f) * 0.5f;
			return lerp_t(t, color0_, color1_);
		}
	private:
		float frequency_;
		RGB color0_;
		RGB color1_;
		PerlinNoise3D noise_;
	};
}

#endif