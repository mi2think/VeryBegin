/********************************************************************
	created:	2016/05/25
	created:	25:5:2016   15:53
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\renderers\samplerrenderer.cpp
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\renderers
	file base:	samplerrenderer
	file ext:	cpp
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#include "samplerrenderer.h"

#include "../core/integrator.h"
#include "../core/sampler.h"
#include "../core/camera.h"

namespace dopbrt
{
	SamplerRenderer::SamplerRenderer(Sampler* s, Camera* c, 
		SurfaceIntegrator* si, VolumeIntegrator* vi, bool visIds)
		: sampler_(s)
		, camera_(c)
		, surfaceIntegrator_(si)
		, volumeIntegrator_(vi)
		, visualizeObjectIds_(visIds)
	{}

	SamplerRenderer::~SamplerRenderer()
	{
		delete sampler_;
		delete camera_;
		delete surfaceIntegrator_;
		delete volumeIntegrator_;
	}

	void SamplerRenderer::Render(const Scene* scene)
	{
		// allow integrators to do preprocessing for the scene
		surfaceIntegrator_->Preprocess(scene);
		volumeIntegrator_->Preprocess(scene);

		// allocate and initialize
		Sample* sample = new Sample(sampler_, surfaceIntegrator_,
			volumeIntegrator_, scene);

		// create and launch SamplerRendererTask for rendering image
		
		// compute number of task to create for rendering
		Film* film = camera_->GetFilm();
		int pixels = film->GetXResolution() * film->GetYResolution();
		// at least 32 tasks for each processing core,
		// each task work on an approximately 16x16 pixel block
		int tasks = MAX(32 * NumSystemCores(), pixels / (16 * 16));
		tasks = RoundUpPow2(tasks);

		// clean up after rendering and store final image
		delete sample;
		camera_->GetFilm()->WriteImage();
	}

	Spectrum SamplerRenderer::Li(const Scene* scene, const RayDifferential& ray,
		const Sample* sample, RNG& rng, MemoryArena& arena, Intersection* isect, Spectrum* T)
	{

	}
}