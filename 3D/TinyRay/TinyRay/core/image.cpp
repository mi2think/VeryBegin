/********************************************************************
	created:	2016/07/05
	created:	5:7:2016   22:02
	filename: 	D:\Code\TinyRay\TinyRay\core\image.cpp
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	image
	file ext:	cpp
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	
*********************************************************************/
#include "image.h"
#include "rgb.h"
#include "log.h"

#include <fstream>
#include <cassert>
#include <cmath>

namespace tinyray
{
	PPM::PPM()
		: width_(0)
		, height_(0)
	{}

	PPM::PPM(int width, int height)
		: raster_(width * height)
		, width_(width)
		, height_(height)
	{
	}

	PPM::PPM(int width, int height, const RGB& rgb)
		: raster_(width * height_)
		, width_(width)
		, height_(height)
	{
		for (int i = 0; i < raster_.Length(); ++i)
			raster_[i] = rgb;
	}

	PPM::~PPM()
	{}

	void PPM::GammaCorrect(float gamma)
	{
		float power = 1.0f / gamma;
		for (int i = 0; i < raster_.Length(); ++i)
		{
			auto& rgb = raster_[i];
			rgb = RGB(pow(rgb.r_, power), pow(rgb.g_, power), pow(rgb.b_, power));
		}
	}

	void PPM::SavePPM(const char* fileName)
	{
		std::ofstream ofs(fileName, std::ios::out | std::ios::binary);
		ofs << "P6\n" << width_ << " " << height_ << "\n255\n";
		for (int i = 0; i < raster_.Length(); ++i)
		{
			const auto& rgb = raster_[i];
			float r = (rgb.r_ > 1.0f ? 1.0f : rgb.r_);
			float g = (rgb.g_ > 1.0f ? 1.0f : rgb.g_);
			float b = (rgb.b_ > 1.0f ? 1.0f : rgb.b_);
			ofs << (unsigned char)(r * 255)
				<< (unsigned char)(g * 255)
				<< (unsigned char)(b * 255);
		}
		ofs.close();
	}

	bool PPM::ReadPPM(const char* fileName)
	{
		std::ifstream ifs(fileName);
		if (!ifs)
		{
			LOG_ERROR("can not open file %s for read!", fileName);
			return false;
		}
		std::string fileType;
		int num;

		ifs >> fileType >> width_ >> height_ >> num;
		raster_.Resize(width_ * height_);

		unsigned char r, g, b;
		RGB* pixel = &(raster_[0]);
		for (int i = 0; i < width_ * height_; ++i)
		{
			ifs >> r >> g >> b;
			pixel[i] = RGB(r / 255.0f, g / 255.0f, b / 255.0f);
		}
		return true;
	}
}
