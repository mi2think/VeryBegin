/********************************************************************
	created:	2014/06/15
	created:	15:6:2014   10:23
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpMath.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpMath
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Some math Def
*********************************************************************/
#ifndef __DP_MATH__
#define __DP_MATH__

#include <float.h>
#include <cassert>

inline float Lerp(float t, float v1, float v2)
{
	return (1.0f - t) * v1 + t * v2;
}

namespace dopixel
{
	namespace math
	{

#define isnan _isnan
#define isinf(f) (!_finite((f)))

#define M_PI	3.14159265358979323846f
#define INV_PI	0.31830988618379067154f

#define angle2radian(a) (float)((a) * M_PI / 180)
#define radian2angle(r) (float)((r) * 180 / M_PI)

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))

		// floating point comparison
#define EPSILON_E3 (float)(1E-3)
#define EPSILON_E5 (float)(1E-5)
#define FCMP(a,b) ( (fabs(a-b) < EPSILON_E3) ? true : false)

		// swap
		template<typename T>
		inline void swap_t(T& a, T& b) { T t = a; a = b; b = t; }

		// equal
		inline bool equal(const float& a, const float& b)
		{
			return abs(a - b) < EPSILON_E5;
		}
		template<typename T>
		inline bool equal_t(const T& a, const T& b) { return a == b; }
		inline bool equal_t(const float& a, const float& b) { return equal(a, b); }

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
	}
}

#endif
