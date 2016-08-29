/********************************************************************
	created:	2016/07/26
	created:	26:7:2016   15:04
	filename: 	D:\Code\TinyRay\TinyRay\math\rng.h
	file path:	D:\Code\TinyRay\TinyRay\math
	file base:	rng
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Random Number Generator
*********************************************************************/
#ifndef __RNG_H__
#define __RNG_H__

#include <random>

namespace tinyray
{
	class RNG
	{
	public:
		RNG(unsigned int seed);

		void Seed(unsigned int seed);

		int RandomInt() const;
		int RandomInt(int imin, int imax) const;
		void RandomInt(int* buffer, int bufferSize, int imin, int imax) const;

		unsigned int RandomUInt() const;
		unsigned int RandomUInt(unsigned int umin, unsigned umax) const;
		void RandomUInt(unsigned int* buffer, int bufferSize, unsigned int umin, unsigned umax) const;

		float RandomFloat() const;
		float RandomFloat(float fmin, float fmax) const;
		void RandomFloat(float* buffer, int bufferSize, float fmin, float fmax) const;
	private:
		mutable std::default_random_engine engine_;
	};
}

#endif