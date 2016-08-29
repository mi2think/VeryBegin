/********************************************************************
	created:	2016/05/25
	created:	25:5:2016   14:48
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\renderer.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	renderer
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __CORE_RENDERER_H__
#define __CORE_RENDERER_H__

#include "pbrt.h"

namespace dopbrt
{
	class Renderer
	{
	public:
		virtual ~Renderer();
		
		// compute an image of scene
		virtual void Render(const Scene* scene) = 0;

		// return incident radiance along the given ray
		// sample, rng: for Monte Carlo integration
		// arena: for efficient allocation for small memory used in compute radiance along ray
		virtual Spectrum Li(const Scene* scene, const RayDifferential& ray,
			const Sample* sample, RNG& rng, MemoryArena& arena,
			Intersection* isect = nullptr, Spectrum* T = nullptr) const = 0;

		// return the fraction of light that is attenuated by volumetric scattering along ray
		virtual Spectrum Transmittance(const Scene* scene, const RayDifferential& ray,
			const Sample* sample, RNG& rng, MemoryArena& arena) const = 0;
	};
}

#endif
