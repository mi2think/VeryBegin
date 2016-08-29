/********************************************************************
	created:	2016/05/25
	created:	25:5:2016   16:31
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\sampler.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	sampler
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __CORE_SAMPLER_H__
#define __CORE_SAMPLER_H__

#include "pbrt.h"

namespace dopbrt
{
	struct CameraSample
	{

	};

	class Sample : public CameraSample
	{
	public:
		Sample(Sampler* sampler, SurfaceIntegrator* si, VolumeIntegrator* vi,
			const Scene* scene);
	};
}

#endif