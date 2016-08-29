/********************************************************************
	created:	2016/05/25
	created:	25:5:2016   17:19
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\parallel.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	parallel
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __CORE_PARALLEL_H__
#define __CORE_PARALLEL_H__

#include "pbrt.h"
#include <atomic>

namespace dopbrt
{
	typedef std::atomic_int32_t AtomicInt32;

	inline int32_t AtomicAdd(AtomicInt32* v, int32_t delta)
	{
		return std::atomic_fetch_add(v, delta) + delta;
	}


	class Task
	{
	public:
		virtual ~Task();
		virtual void Run() = 0;
	};


	int NumSystemCores();
}

#endif