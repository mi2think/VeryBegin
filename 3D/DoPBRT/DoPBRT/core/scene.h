/********************************************************************
	created:	2016/05/24
	created:	24:5:2016   20:08
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\scene.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	scene
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	Scene
*********************************************************************/
#ifndef __CORE_SCENE_H__
#define __CORE_SCENE_H__

#include "pbrt.h"
#include "primitive.h"

namespace dopbrt
{
	class Scene
	{
	public:
		Scene(Primitive* accel, const vector<Light*>& lts, VolumeRegion vr);
		~Scene();

		bool Intersect(const Ray& ray, Intersection* isect) const
		{
			bool hit = aggregate_->Intersect(ray, isect);
			return hit;
		}

		bool IntersectP(const Ray& ray) const
		{
			bool hit = aggregate_->IntersectP(ray);
			return hit;
		}

		const BBox& WorldBound() const { return bound_; }
	private:
		Primitive* aggregate_;
		vector<Light*> lights_;
		VolumeRegion* volumeRegion_;
		BBox bound_;
	};
}

#endif
