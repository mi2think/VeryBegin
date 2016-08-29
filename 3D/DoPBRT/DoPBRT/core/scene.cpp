/********************************************************************
	created:	2016/05/25
	created:	25:5:2016   10:36
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\scene.cpp
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	scene
	file ext:	cpp
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	Scene
*********************************************************************/
#include "scene.h"

namespace dopbrt
{
	Scene::Scene(Primitive* accel, const vector<Light*>& lts, VolumeRegion vr)
		: aggregate_(accel)
		, lights_(lts)
		, volumeRegion_(vr)
	{
		bound_ = aggregate_->WorldBound();
		if (volumeRegion_)
			bound_->Add(volumeRegion_->WorldBound());
	}

	Scene::~Scene()
	{
		delete aggregate_;
		delete volumeRegion_;
		for (auto& light : lights_)
			delete light;
	}
}
