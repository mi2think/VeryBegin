/********************************************************************
	created:	2016/05/25
	created:	25:5:2016   17:21
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\parallel.cpp
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	parallel
	file ext:	cpp
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#include "parallel.h"

namespace dopbrt
{
	Task::~Task()
	{}


	int NumSystemCores()
	{
		if (g_PbrtOptions.cores_ > 0)
			return g_PbrtOptions.cores_;

#if defined(PBRT_IS_WINDOWS)
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		return sysinfo.dwNumberOfProcessors;
#else
#endif
	}
}