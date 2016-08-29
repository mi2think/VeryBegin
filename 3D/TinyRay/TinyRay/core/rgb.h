/********************************************************************
	created:	2016/07/05
	created:	5:7:2016   20:24
	filename: 	D:\Code\TinyRay\TinyRay\core\rgb.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	rgb
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	
*********************************************************************/
#ifndef __RGB_H__
#define __RGB_H__

namespace tinyray
{
	class RGB
	{
	public:
		float r_;
		float g_;
		float b_;

		RGB() : r_(0.0f), g_(0.0f), b_(0.0f) {}
		RGB(float r, float g, float b) : r_(r), g_(g), b_(b) {}
		RGB(const RGB& rgb) : r_(rgb.r_), g_(rgb.g_), b_(rgb.b_) {}

		void SetRed(float r) { r_ = r; }
		void SetGreen(float g) { g_ = g; }
		void SetBlue(float b) { b_ = b; }

		void Clamp()
		{
			if (r_ > 1.0f) r_ = 1.0f;
			if (g_ > 1.0f) g_ = 1.0f;
			if (b_ > 1.0f) b_ = 1.0f;

			if (r_ < 0.0f) r_ = 0.0f;
			if (g_ < 0.0f) g_ = 0.0f;
			if (b_ < 0.0f) b_ = 0.0f;
		}

		// no clamp operator
		RGB operator+() const { return *this; }
		RGB operator-() const { return RGB(-r_, -g_, -b_); }

		RGB& operator*= (float k) { r_ *= k; g_ *= k; b_ *= k; return *this; }
		RGB  operator*  (float k) const { RGB rgb = *this; rgb *= k; return rgb; }
		RGB& operator/= (float k) { float f = 1.0f / k;  *this *= f; return *this; }
		RGB  operator/  (float k) const { RGB rgb = *this; rgb /= k; return rgb; }

		RGB& operator+=(const RGB& rgb) { r_ += rgb.r_; g_ += rgb.g_; b_ += rgb.b_; return *this; }
		RGB  operator+ (const RGB& rgb) const { RGB rgbt = rgb; rgbt += *this; return rgbt; }
		RGB& operator-=(const RGB& rgb) { r_ -= rgb.r_; g_ -= rgb.g_; b_ -= rgb.b_; return *this; }
		RGB  operator- (const RGB& rgb) const { RGB rgbt = rgb; rgbt -= *this; return rgbt; }
		RGB& operator*=(const RGB& rgb) { r_ *= rgb.r_; g_ *= rgb.g_; b_ *= rgb.b_; return *this; }
		RGB  operator* (const RGB& rgb) const { RGB rgbt = rgb; rgbt *= *this; return rgbt; }
		RGB& operator/=(const RGB& rgb) { r_ /= rgb.r_; g_ /= rgb.g_; b_ /= rgb.b_; return *this; }
		RGB  operator/ (const RGB& rgb) const { RGB rgbt = rgb; rgbt /= *this; return rgbt; }
	};

	inline RGB operator*(float k, const RGB& rgb)
	{
		RGB rgbt = rgb;
		rgbt *= k;
		return rgbt;
	}
}


#endif