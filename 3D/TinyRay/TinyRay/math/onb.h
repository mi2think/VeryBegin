/********************************************************************
	created:	2016/07/17
	created:	17:7:2016   22:18
	filename: 	D:\Code\TinyRay\TinyRay\math\onb.h
	file path:	D:\Code\TinyRay\TinyRay\math
	file base:	onb
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	Orthonormal Basis
*********************************************************************/
#ifndef __ONB_H__
#define __ONB_H__

#include "vector3.h"

namespace tinyray
{
	class ONB
	{
	public:
		Vector3f u_;
		Vector3f v_;
		Vector3f w_;

		ONB() {}
		ONB(const Vector3f& x, const Vector3f& y, const Vector3f& z)
			: u_(x)
			, v_(y)
			, w_(z)
		{}

		void Set(const Vector3f& x, const Vector3f& y, const Vector3f& z)
		{
			u_ = x;
			v_ = y;
			w_ = z;
		}

		void InitFromU(const Vector3f& u);
		void InitFromV(const Vector3f& v);
		void InitFromW(const Vector3f& w);

		// calculate the ONB from two vectors
		// the first one is fixed vector(it is just normalized)
		// the second is normalized and its direction can be adjusted
		void InitFromUV(const Vector3f& u, const Vector3f& v);
		void InitFromVU(const Vector3f& v, const Vector3f& u);

		void InitFromUW(const Vector3f& u, const Vector3f& w);
		void InitFromWU(const Vector3f& w, const Vector3f& u);

		void InitFromVW(const Vector3f& v, const Vector3f& w);
		void InitFromWV(const Vector3f& w, const Vector3f& v);
	};
}

#endif