/********************************************************************
	created:	2016/07/06
	created:	6:7:2016   22:38
	filename: 	D:\Code\TinyRay\TinyRay\math\math_fwd.h
	file path:	D:\Code\TinyRay\TinyRay\math
	file base:	math_fwd
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Math
*********************************************************************/
#ifndef __MATH_FWD_H__
#define __MATH_FWD_H__

#include <float.h>
#include <cassert>
#include <cstring>
#include <cmath>

namespace tinyray
{
	template <typename T> class Vector2;
	typedef Vector2<float> Vector2f;
	typedef Vector2<int>   Vector2i;

	template <typename T> class Vector3;
	typedef Vector3<float> Vector3f;

	template <typename T> class Vector4;
	typedef Vector4<float> Vector4f;

	template <typename T> class Matrix33;
	typedef Matrix33<float> Matrix33f;

	template <typename T> class Matrix44;
	typedef Matrix44<float> Matrix44f;

#define isnan _isnan
#define isinf(f) (!_finite((f)))

#define M_PI	3.14159265358979323846f
#define INV_PI	0.31830988618379067154f

#define EPSILON_E3 (float)(1E-3)
#define EPSILON_E5 (float)(1E-5)
#define ABS(a) (fabs(a))
#define FCMP(a,b) ( (fabs(a-b) < EPSILON_E3) ? true : false)

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))

#define angle2radian(a) (float)((a) * M_PI / 180)
#define radian2angle(r) (float)((r) * 180 / M_PI)

#define floor2int(a) (int)(std::floor((a)))
#define ceil2int(a)  (int)(std::ceil((a))

	// lerp, t:[0, 1]
	template<typename T>
	inline T lerp_t(float t, const T& v1, const T& v2) { return (1.0f - t) * v1 + t * v2; }
	// cos lerp, t:[0, 1]
	template<typename T>
	inline T coslerp_t(float t, const T& v1, const T& v2)
	{
		// remap t to [0, Pi], so cos(t):[1, -1], then scale it to [0, 1]
		float remap_t = (1 - cosf(t * M_PI)) * 0.5f;
		return lerp_t(remap_t, v1, v2);
	}
	// smooth step lerp, t:[0, 1]
	template<typename T>
	inline T smoothsteplerp_t(float t, const T& v1, const T& v2)
	{
		float remap_t = t * t * (3 - 2 * t);
		return lerp_t(remap_t, v1, v2);
	}

	// swap
	template<typename T>
	inline void swap_t(T& a, T& b) { T t = a; a = b; b = t; }

	// equal
	template<typename T>
	inline bool equal_t(const T& a, const T& b) { return a == b; }
	inline bool equal_t(const float& a, const float& b) { return ABS(a - b) < EPSILON_E5; }

	// clamp
	template <typename T>
	inline T clamp_t(const T& val, const T& minVal, const T& maxVal)
	{
		return (val < minVal ? minVal : (val > maxVal ? maxVal : val));
	}

	inline void sincos(float& sin_t, float cos_t, float radian)
	{
		sin_t = sinf(radian);
		cos_t = cosf(radian);
	}

	inline bool quadratic(float A, float B, float C, float *t0, float *t1)
	{
		// for equation A * t^2 + B * t + C = 0, slove for t values may two possible solutions:
		// t0 = (-B - sqrt(B^2 - 4AC)) / (2A)
		// t1 = (-B + sqrt(B^2 - 4AC)) / (2A)
		// but here in pbrt, it use another way for avoid cancellation error
		// (if B Approximately equal to +-sqrt(B^2 - 4AC)).

		// find quadratic discriminant
		float discrim = B * B - 4.f * A * C;
		if (discrim < 0.0f) return false;
		float rootDiscrim = sqrtf(discrim);

		// compute quadratic _t_ values
		float q;
		if (B < 0) q = -0.5f * (B - rootDiscrim);
		else       q = -0.5f * (B + rootDiscrim);
		*t0 = q / A;
		*t1 = C / q;
		if (*t0 > *t1) swap_t(*t0, *t1);
		return true;
	}

	// Perlin noise data
#define NOISE_PERM_SIZE 256
	extern const int NoisePerm[NOISE_PERM_SIZE * 2];
}

#endif
