/********************************************************************
	created:	2016/07/26
	created:	26:7:2016   10:19
	filename: 	D:\Code\TinyRay\TinyRay\core\valuenoise.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	valuenoise
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Value Noise
*********************************************************************/
#ifndef __VALUE_NOISE_H__
#define __VALUE_NOISE_H__

#include "math/math_fwd.h"
#include "math/rng.h"
#include "math/vector2.h"

namespace tinyray
{
	template<typename Func>
	class ValueNoise1D
	{
	public:
		enum { TableSize = NOISE_PERM_SIZE, TableSizeMask = TableSize - 1 };

		ValueNoise1D(unsigned int seed = 2016726)
			: rng_(seed)
		{
			rng_.RandomFloat(r_, TableSize, 0.0f, 1.0f);
		}

		// evaluate the noise function at position x
		float Eval(float x)
		{
			int ix = floor2int(x);
			float t = x - ix;
			// modulo
			int xmin = ix & TableSizeMask;
			int xmax = (ix + 1) & TableSizeMask;
			return Func(t, xmin, xmax);
		}
	private:
		RNG rng_;
		float r_[TableSize];
	};

	class ValueNoise2D
	{
	public:
		enum { TableSize = NOISE_PERM_SIZE, TableSizeMask = TableSize - 1 };

		ValueNoise2D(unsigned int seed = 2016730)
			: rng_(seed)
		{
			rng_.RandomFloat(r_, TableSize, 0.0f, 1.0f);
		}

		float Eval(const Vector2f& pt) const
		{
			return Eval(pt.x, pt.y);
		}

		float Eval(float x, float y) const
		{
			int ix = floor2int(x);
			int iy = floor2int(y);

			float tx = x - ix;
			float ty = y - iy;

			int rx0 = ix & TableSizeMask;
			int rx1 = (rx0 + 1) & TableSizeMask;
			int ry0 = iy & TableSizeMask;
			int ry1 = (ry0 + 1) & TableSizeMask;

			// random values at corners of cell using permutation table
			const float& c00 = r_[NoisePerm[NoisePerm[rx0] + ry0]];
			const float& c01 = r_[NoisePerm[NoisePerm[rx0] + ry1]];
			const float& c10 = r_[NoisePerm[NoisePerm[rx1] + ry0]];
			const float& c11 = r_[NoisePerm[NoisePerm[rx1] + ry1]];

			// interpolate values along x axis
			float nx0 = smoothsteplerp_t(tx, c00, c10);
			float nx1 = smoothsteplerp_t(tx, c01, c11);

			// interpolate values along y axis
			return smoothsteplerp_t(ty, nx0, nx1);
		}
	private:
		RNG rng_;
		float r_[TableSize];
	};
}

#endif
