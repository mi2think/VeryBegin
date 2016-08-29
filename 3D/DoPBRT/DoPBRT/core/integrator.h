/********************************************************************
	created:	2016/05/25
	created:	25:5:2016   16:08
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\integrator.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	integrator
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __CORE_INTEGRATOR_H__
#define __CORE_INTEGRATOR_H__

#include "pbrt.h"

namespace dopbrt
{
	class Integrator
	{
	public:
		virtual ~Integrator();

		virtual void Preprocess(const Scene* scene, const Camera* camera,
			const Renderer* renderer)
		{}

		virtual void RequestSamples(Sampler* sampler, Sample* sample,
			const Scene* scene)
		{}
	};

	class SurfaceIntegrator : public Integrator
	{
	public:
	};
}

#endif