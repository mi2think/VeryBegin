/********************************************************************
	created:	2016/08/01
	created:	1:8:2016   10:45
	filename: 	D:\Code\TinyRay\TinyRay\core\perlinnoise.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	perlinnoise
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Perlin Noise
*********************************************************************/
#ifndef __PERLIN_NOISE_H__
#define __PERLIN_NOISE_H__

#include "math/math_fwd.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/rng.h"

namespace tinyray
{
	class PerlinNoise2D
	{
	public:
		enum { TableSize = NOISE_PERM_SIZE, TableSizeMask = TableSize - 1 };

		PerlinNoise2D()
			: rng_(20160802)
		{
			float r[TableSize];
			rng_.RandomFloat(r, TableSize, 0.0f, 2 * M_PI);

			for (int i = 0; i < TableSize; ++i)
			{
				float x = cos(r[i]);
				float y = sin(r[i]);
				gradients_[i] = Vector2f(x, y);
			}
		}

		float Quintic(float t) const
		{
			// 6t^5 - 15t^4 + 10t^3
			float t3 = t * t * t;
			float t4 = t3 * t;
			return 6.0f * t4 * t - 15.0f * t4 + 10.0f * t3;
		}

		float Grad(int x, int y, float dx, float dy) const
		{
			int h = NoisePerm[NoisePerm[x] + y];
			const Vector2f& v = gradients_[h];
			return DotProduct(v, Vector2f(dx, dy));
		}

		float Eval(const Vector2f& pt) const
		{
			return Eval(pt.x, pt.y);
		}

		float Eval(float x, float y) const
		{
			int ix0 = floor2int(x) & TableSizeMask;
			int iy0 = floor2int(y) & TableSizeMask;

			int ix1 = (ix0 + 1) & TableSizeMask;
			int iy1 = (iy0 + 1) & TableSizeMask;

			float tx = x - floor2int(x);
			float ty = y - floor2int(y);

			float u = Quintic(tx);
			float v = Quintic(ty);

			// generate vectors going from the grid points to p and compute weights
			float w00 = Grad(ix0, iy0, tx, ty);
			float w10 = Grad(ix1, iy0, tx - 1, ty); // tx + ix0 - ix1 = tx - 1
			float w01 = Grad(ix0, iy1, tx, ty - 1);
			float w11 = Grad(ix1, iy1, tx - 1, ty - 1);

			// compute bilinear interpolation of weights
			float x0 = lerp_t(u, w00, w10);
			float x1 = lerp_t(u, w01, w11);
			return lerp_t(v, x0, x1);
		}
	private:
		RNG rng_;
		Vector2f gradients_[TableSize];
	};


	class PerlinNoise3D
	{
	public:
		enum { TableSize = NOISE_PERM_SIZE, TableSizeMask = TableSize - 1 };

		PerlinNoise3D() {}

		float Quintic(float t) const 
		{
			// 6t^5 - 15t^4 + 10t^3
			float t3 = t * t * t;
			float t4 = t3 * t;
			return 6.0f * t4 * t - 15.0f * t4 + 10.0f * t3;
		}

		float Grad(int x, int y, int z, float dx, float dy, float dz) const
		{
			// hash of cell corner
			int h = NoisePerm[NoisePerm[NoisePerm[x] + y] + z];
			
			//dot product between vector from cell corners to P with predefined gradient directions
			switch (h & 0xF)
			{
			case 0x0: return  dx + dy; // (1,1,0)
			case 0x1: return -dx + dy; // (-1,1,0)
			case 0x2: return  dx - dy; // (1,-1,0)
			case 0x3: return -dx - dy; // (-1,-1,0)
			case 0x4: return  dx + dz; // (1,0,1)
			case 0x5: return -dx + dz; // (-1,0,1)
			case 0x6: return  dx - dz; // (1,0,-1)
			case 0x7: return -dx - dz; // (-1,0,-1)
			case 0x8: return  dy + dz; // (0,1,1)
			case 0x9: return -dy + dz; // (0,-1,1)
			case 0xA: return  dy - dz; // (0,1,-1)
			case 0xB: return -dy - dz; // (0,-1,-1)
			case 0xC: return  dy + dx; // (1,1,0)
			case 0xD: return  dy - dx; // (-1,1,0)
			case 0xE: return -dy + dz; // (0,-1,1)
			case 0xF: return -dy - dz; // (0,-1,-1)
			default:  return 0; // never happens
			}
		}

		float Eval(const Vector3f& pt) const
		{
			return Eval(pt.x, pt.y, pt.z);
		}

		float Eval(float x, float y, float z) const
		{
			int ix0 = floor2int(x) & TableSizeMask;
			int iy0 = floor2int(y) & TableSizeMask;
			int iz0 = floor2int(z) & TableSizeMask;

			int ix1 = (ix0 + 1) & TableSizeMask;
			int iy1 = (iy0 + 1) & TableSizeMask;
			int iz1 = (iz0 + 1) & TableSizeMask;

			float tx = x - floor2int(x);
			float ty = y - floor2int(y);
			float tz = z - floor2int(z);

			float u = Quintic(tx);
			float v = Quintic(ty);
			float w = Quintic(tz);

			// generate vectors going from the grid points to p and compute weights
			float w000 = Grad(ix0, iy0, iz0, tx, ty, tz);
			float w100 = Grad(ix1, iy0, iz0, tx - 1, ty, tz); // tx + ix0 - ix1 = tx - 1
			float w010 = Grad(ix0, iy1, iz0, tx, ty - 1, tz);
			float w110 = Grad(ix1, iy1, iz0, tx - 1, ty - 1, tz);
			float w001 = Grad(ix0, iy0, iz1, tx, ty, tz - 1);
			float w101 = Grad(ix1, iy0, iz1, tx - 1, ty, tz - 1);
			float w011 = Grad(ix0, iy1, iz1, tx, ty - 1, tz - 1);
			float w111 = Grad(ix1, iy1, iz1, tx - 1, ty - 1, tz - 1);

			// compute trilinear interpolation of weights
			float x00 = lerp_t(u, w000, w100);
			float x10 = lerp_t(u, w010, w110);
			float x01 = lerp_t(u, w001, w101);
			float x11 = lerp_t(u, w011, w111);
			float y0  = lerp_t(v, x00, x10);
			float y1  = lerp_t(v, x01, x11);
			return lerp_t(w, y0, y1);
		}
	};
}

#endif
