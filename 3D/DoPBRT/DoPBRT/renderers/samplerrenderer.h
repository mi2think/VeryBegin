/********************************************************************
	created:	2016/05/25
	created:	25:5:2016   15:20
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\renderers\samplerrenderer.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\renderers
	file base:	samplerrenderer
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __RENDERERS_SAMPLERRENDERER_H__
#define __RENDERERS_SAMPLERRENDERER_H__

#include "../core/pbrt.h"
#include "../core/renderer.h"
#include "../core/parallel.h"

namespace dopbrt
{
	class SamplerRenderer : public Renderer
	{
	public:
		SamplerRenderer(Sampler* s, Camera* c,
			SurfaceIntegrator* si, VolumeIntegrator* vi, bool visIds);
		~SamplerRenderer();

		virtual void Render(const Scene* scene) override;

		virtual Spectrum Li(const Scene* scene, const RayDifferential& ray,
			const Sample* sample, RNG& rng, MemoryArena& arena,
			Intersection* isect = nullptr, Spectrum* T = nullptr) const override;

		virtual Spectrum Transmittance(const Scene* scene, const RayDifferential& ray,
			const Sample* sample, RNG& rng, MemoryArena& arena) const override;
	private:
		// 
		bool visualizeObjectIds_;

		// 1. for choosing the points on image plane from which ray are traced
		// 2. for supplying the sample points used by integrators 
		//    in light transport computations
		Sampler* sampler_;

		// camera
		Camera* camera_;

		// compute reflected light from geometric surfaces
		SurfaceIntegrator* surfaceIntegrator_;

		// compute scattering from volumetric primitives
		VolumeIntegrator* volumeIntegrator_;
	};

	class SamplerRendererTask : public Task
	{
	public:
		SamplerRendererTask(const Scene* scene, Renderer* render, Camera* camera,
			Sampler* sampler, Sample* sample, int taskNum, int taskCount);
		
		void Run();
	private:
		const Scene* scene_;
		const Renderer* renderer_;
		Camera* camera_;
		Sampler* mainSampler_;
		Sample* origSample_;
		int taskNum_;
		int taskCount_;
	};
}


#endif