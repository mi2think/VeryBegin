/********************************************************************
	created:	2016/05/25
	created:	25:5:2016   17:07
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\film.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	film
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __CORE_FILM_H__
#define __CORE_FILM_H__

#include "pbrt.h"

namespace dopbrt
{
	class Film
	{
	public:
		Film(int xres, int yres)
			: xResolution_(xres)
			, yResolution_(yres)
		{
		}

		int GetXResolution() const { return xResolution_; }
		int GetYResolution() const { return yResolution_; }

		virtual void WriteImage(float splatScale = 1.f) = 0;
	private:
		const int xResolution_;
		const int yResolution_;
	};
}

#endif

