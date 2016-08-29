/********************************************************************
	created:	2016/07/05
	created:	5:7:2016   21:53
	filename: 	D:\Code\TinyRay\TinyRay\core\image.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	image
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	
*********************************************************************/
#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "array.h"

namespace tinyray
{
	class RGB;

	// Portable PixMaps
	class PPM
	{
	public:
		PPM();
		// init raster to default rgb
		PPM(int width, int height);
		// init raster to background rgb
		PPM(int width, int height, const RGB& rgb);
		~PPM();

		int GetWidth() const { return width_; }
		int GetHeight() const { return height_; }
		void GammaCorrect(float gamma);
		void SavePPM(const char* fileName);
		bool ReadPPM(const char* fileName);
		Array<RGB>& GetRaster() { return raster_; }
	private:
		Array<RGB> raster_;
		int width_;
		int height_;
	};
}

#endif
