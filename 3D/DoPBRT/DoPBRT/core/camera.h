/********************************************************************
	created:	2016/05/25
	created:	25:5:2016   17:05
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\camera.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	camera
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __CORE_CAMERA_H__
#define __CORE_CAMERA_H__

#include "pbrt.h"

namespace dopbrt
{
	class Camera
	{
	public:

		Film* GetFilm() const { return film_; }
	private:
		Film* film_;
	};
}

#endif