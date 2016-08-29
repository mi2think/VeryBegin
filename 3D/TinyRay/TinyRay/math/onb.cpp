/********************************************************************
	created:	2016/07/17
	created:	17:7:2016   22:37
	filename: 	D:\Code\TinyRay\TinyRay\math\onb.h
	file path:	D:\Code\TinyRay\TinyRay\math
	file base:	onb
	file ext:	cpp
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	Orthonormal Basis
*********************************************************************/
#include "onb.h"
#include "math_fwd.h"

namespace tinyray
{
	void ONB::InitFromU(const Vector3f& u)
	{
		Vector3f n(1.0f, 0.0f, 0.0f);
		Vector3f m(0.0f, 1.0f, 0.0f);

		u_ = u;
		u_.Normalize();
		v_ = CrossProduct(u_, n);
		if (v_.LengthSQ() < EPSILON_E3)
			v_ = CrossProduct(u_, m);
		w_ = CrossProduct(u_, v_);
	}

	void ONB::InitFromV(const Vector3f& v)
	{
		Vector3f n(1.0f, 0.0f, 0.0f);
		Vector3f m(0.0f, 1.0f, 0.0f);

		v_ = v;
		v_.Normalize();
		u_ = CrossProduct(v_, n);
		if (u_.LengthSQ() < EPSILON_E3)
			u_ = CrossProduct(v_, m);
		w_ = CrossProduct(u_, v_);
	}

	void ONB::InitFromW(const Vector3f& w)
	{
		Vector3f n(1.0f, 0.0f, 0.0f);
		Vector3f m(0.0f, 1.0f, 0.0f);

		w_ = w;
		w_.Normalize();
		u_ = CrossProduct(w_, n);
		if (u_.LengthSQ() < EPSILON_E3)
			u_ = CrossProduct(w_, m);
		v_ = CrossProduct(w_, u_);
	}

	void ONB::InitFromUV(const Vector3f& u, const Vector3f& v)
	{
		u_ = u;
		u_.Normalize();
		w_ = CrossProduct(u, v);
		w_.Normalize();
		v_ = CrossProduct(w_, u_);
	}

	void ONB::InitFromVU(const Vector3f& v, const Vector3f& u)
	{
		v_ = v;
		v_.Normalize();
		w_ = CrossProduct(u, v);
		w_.Normalize();
		u_ = CrossProduct(v_, w_);
	}

	void ONB::InitFromUW(const Vector3f& u, const Vector3f& w)
	{
		u_ = u;
		u_.Normalize();
		v_ = CrossProduct(w, u);
		v_.Normalize();
	}

	void ONB::InitFromWU(const Vector3f& w, const Vector3f& u)
	{
		w_ = w;
		w_.Normalize();
		v_ = CrossProduct(w_, u_);
		v_.Normalize();
		u_ = CrossProduct(v_, w_);
	}

	void ONB::InitFromVW(const Vector3f& v, const Vector3f& w)
	{
		v_ = v;
		v_.Normalize();
		u_ = CrossProduct(v, w);
		u_.Normalize();
		w_ = CrossProduct(u_, v_);
	}

	void ONB::InitFromWV(const Vector3f& w, const Vector3f& v)
	{
		w_ = w;
		w_.Normalize();
		u_ = CrossProduct(v, w);
		u_.Normalize();
		v_ = CrossProduct(w_, u_);
	}
}
