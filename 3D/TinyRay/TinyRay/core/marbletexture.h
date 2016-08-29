/********************************************************************
	created:	2016/08/03
	created:	3:8:2016   15:42
	filename: 	D:\Code\TinyRay\TinyRay\core\marbletexture.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	marbletexture
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".

	modify:

	purpose:	Noise Texture
*********************************************************************/
#ifndef __MARBLE_TEXTURE_H__
#define __MARBLE_TEXTURE_H__

#include "texture.h"
#include "perlinnoise.h"

namespace tinyray
{
	class MarbleTexture : public Texture
	{
	public:
		MarbleTexture(float frequency, float frequencyMult, float amplitudeMult, 
			int layers, const RGB& c0, const RGB& c1)
			: frequency_(frequency)
			, frequencyMult_(frequencyMult)
			, amplitudeMult_(amplitudeMult)
			, layers_(layers)
			, color0_(c0)
			, color1_(c1)
		{}

		RGB Value(const Vector2f& uv, const Vector3f& pt) const
		{
			Vector3f ptNoise = pt;
			float amplitude = 1;
			float noiseValue = 0;

			for (int i = 0; i < layers_; ++i)
			{
				noiseValue += (noise_.Eval(ptNoise) + 1) * 0.5f * amplitude;
				ptNoise *= frequencyMult_;
				amplitude *= amplitudeMult_;
			}

			// we displace the value i used in sin expression by noiseValue * 100
			float t = (sin((pt.x + noiseValue * 100) * 2 * M_PI / 200.0f) + 1) / 2.0f;
			return lerp_t(t, color0_, color1_);
		}
	private:
		RGB color0_;
		RGB color1_;
		int layers_;
		float frequency_;
		float frequencyMult_;
		float amplitudeMult_;
		PerlinNoise3D noise_;
	};
}

#endif