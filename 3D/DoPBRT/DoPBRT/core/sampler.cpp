/********************************************************************
	created:	2016/05/25
	created:	25:5:2016   16:37
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\sampler.cpp
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	sampler
	file ext:	cpp
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#include "sampler.h"

namespace dopbrt
{
	Sample::Sample(Sampler* sampler, SurfaceIntegrator* si, VolumeIntegrator* vi, 
		const Scene* scene)
	{
		if (si)
		{
			si->RequestSamples(sampler, this, scene);
		}
		if (vi)
		{
			vi->RequestSamples(sampler, this, scene);
		}
	}

}