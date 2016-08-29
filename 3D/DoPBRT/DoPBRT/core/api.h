/********************************************************************
	created:	2016/05/18
	created:	18:5:2016   18:02
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\api.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	api
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __CORE_API_H__
#define __CORE_API_H__

#include "pbrt.h"

namespace dopbrt
{
	void pbrtInit(const Options& options);
	void pbrtCleanup();
}


#endif