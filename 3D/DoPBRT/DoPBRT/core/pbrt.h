/********************************************************************
	created:	2016/05/18
	created:	18:5:2016   17:47
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\pbrt.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	pbrt
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
		1. add namespace dopbrt

	purpose:	
*********************************************************************/
#ifndef __CORE_PBRT_H__
#define __CORE_PBRT_H__

#include <functional>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <cassert>
#include <cstring>

// my lib
#include "../mylib/DpLog.h"
#include "../mylib/DpMath.h"
#include "../mylib/DpVector3.h"
#include "../mylib/DpVector4.h"
#include "../mylib/DpAABB.h"
#include "../mylib/DpMatrix44.h"
#include "../mylib/DpMatrix33.h"
#include "../mylib/DpQuaternion.h"
using namespace dopixel;
using namespace dopixel::math;

typedef AABB BBox;
typedef Vector3f Point;
typedef Vector3f Normal;

#if _MSC_VER >= 1600
#include <stdint.h>
#else
typedef signed __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef signed __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef signed __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif // _MSC_VER >= 1600

namespace dopbrt
{
	using std::string;
	using std::wstring;
	using std::vector;
	using std::map;
	using std::pair;

	struct Options
	{
		Options()
			: cores_(0)
			, quickRender_(false)
			, quiet_(false)
			, verbose_(false)
		{
		}
		int cores_;
		bool quickRender_;
		bool quiet_;
		bool verbose_;
		bool openWindow_;
		string imageFile_;
	};

	extern Options g_PbrtOptions;

	class Shape;
	class Ray;
	class RayDifferential;
	class Intersection;
	class Primitive;
	class Light;
	class VolumeRegion;
	class Intersection;
	class Scene;
	class Camera;
	class CameraSample;
	class RGBSpectrum;
	class Sample;
	class Sampler;
	class RNG;
	class SurfaceIntegrator;
	class VolumeIntegrator;
	class MemoryArena;

	typedef RGBSpectrum Spectrum;


	inline uint32_t RoundUpPow2(uint32_t v) {
		v--;
		v |= v >> 1;    v |= v >> 2;
		v |= v >> 4;    v |= v >> 8;
		v |= v >> 16;
		return v + 1;
	}

	inline bool Quadratic(float A, float B, float C, float *t0, float *t1)
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
}

#endif
