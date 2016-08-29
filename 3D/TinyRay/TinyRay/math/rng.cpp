/********************************************************************
	created:	2016/07/26
	created:	26:7:2016   15:05
	filename: 	D:\Code\TinyRay\TinyRay\math\rng.cpp
	file path:	D:\Code\TinyRay\TinyRay\math
	file base:	rng
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Random Number Generator
*********************************************************************/
#include "rng.h"

namespace tinyray
{
	RNG::RNG(unsigned int seed)
		: engine_(seed)
	{
	}

	void RNG::Seed(unsigned int seed)
	{
		engine_.seed(seed);
	}

	int RNG::RandomInt() const
	{
		std::uniform_int_distribution<> u;
		return u(engine_);
	}

	int RNG::RandomInt(int imin, int imax) const
	{
		std::uniform_int_distribution<> u(imin, imax);
		return u(engine_);
	}

	void RNG::RandomInt(int* buffer, int bufferSize, int imin, int imax) const
	{
		std::uniform_int_distribution<> u(imin, imax);
		for (int i = 0; i < bufferSize; ++i)
		{
			*(buffer + i) = u(engine_);
		}
	}

	unsigned int RNG::RandomUInt() const
	{
		std::uniform_int_distribution<unsigned int> u;
		return u(engine_);
	}

	unsigned int RNG::RandomUInt(unsigned int umin, unsigned umax) const
	{
		std::uniform_int_distribution<unsigned int> u(umin, umax);
		return u(engine_);
	}

	void RNG::RandomUInt(unsigned int* buffer, int bufferSize, unsigned int umin, unsigned umax) const
	{
		std::uniform_int_distribution<unsigned int> u(umin, umax);
		for (int i = 0; i < bufferSize; ++i)
		{
			*(buffer + i) = u(engine_);
		}
	}

	float RNG::RandomFloat() const
	{
		std::uniform_real_distribution<float> u;
		return u(engine_);
	}

	float RNG::RandomFloat(float fmin, float fmax) const
	{
		std::uniform_real_distribution<float> u(fmin, fmax);
		return u(engine_);
	}

	void RNG::RandomFloat(float* buffer, int bufferSize, float fmin, float fmax) const
	{
		std::uniform_real_distribution<float> u(fmin, fmax);
		for (int i = 0; i < bufferSize; ++i)
		{
			*(buffer + i) = u(engine_);
		}
	}
}