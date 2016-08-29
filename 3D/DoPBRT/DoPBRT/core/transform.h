/********************************************************************
	created:	2016/06/01
	created:	1:6:2016   10:40
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\transform.h
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	transform
	file ext:	h
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#ifndef __CORE_TRANSFORM_H__
#define __CORE_TRANSFORM_H__

#include "pbrt.h"
#include "geometry.h"

namespace dopbrt
{
	class Transform
	{
	public:
		Transform() { m_.Identity(); mInv_.Identity();}
		Transform(const Matrix44f& m) : m_(m) { MatrixInverse(mInv_, m_);}
		Transform(const Matrix44f& m, const Matrix44f& invm) : m_(m), mInv_(invm) {}
		Transform(const float m[4][4])
		{
			m_ = Matrix44f(m[0][0], m[0][1], m[0][2], m[0][3],
				m[1][0], m[1][1], m[1][2], m[1][3],
				m[2][0], m[2][1], m[2][2], m[2][3],
				m[3][0], m[3][1], m[3][2], m[3][3]);
			MatrixInverse(mInv_, m_);
		}

		bool operator==(const Transform& t) const { return t.m_ == m_ && t.mInv_ == mInv_; }
		bool operator!=(const Transform& t) const { return !operator==(t); }

		bool IsIdentity() const { return m_.IsIdentity(); }
		bool HasScale() const;
		Transform Inverse() const { return Transform(mInv_, m_); }
		Transform operator*(const Transform& t) const;
		bool SwapsHandedness() const;
		const Matrix44f& GetMatrix() const { return m_; }
		const Matrix44f& GetINVMatrix() const { return mInv_; }

		Point ApplyPoint(const Point& pt) const;
		Vector3f ApplyVector(const Vector3f& v) const;
		Normal ApplyNormal(const Normal& n) const;
		Ray ApplyRay(const Ray& r) const;
		BBox ApplyBBox(const BBox& b) const;
		RayDifferential ApplyRayDifferential(const RayDifferential& r) const;

		void ApplyPoint(const Point& pt, Point& ptTrans) const;
		void ApplyVector(const Vector3f& v, Vector3f& vTrans) const;
		void ApplyNormal(const Normal& n, Normal& nTrans) const;
		void ApplyRay(const Ray& r, Ray& rTrans) const;
		void ApplyBBox(const BBox& b, BBox& bTrans) const;
		void ApplyRayDifferential(const RayDifferential& r, RayDifferential& rTrans) const;
	private:
		Matrix44f m_;
		Matrix44f mInv_;
	};

	Transform Translate(const Vector3f& delta);
	Transform Scale(float x, float y, float z);
	Transform RotateX(float angle);
	Transform RotateY(float angle);
	Transform RotateZ(float angle);
	Transform Rotate(float angle, const Vector3f& axis);
	Transform LookAt(const Point& pos, const Point& look, const Vector3f& up);

	//////////////////////////////////////////////////////////////////////////

	class AnimatedTransform
	{
	public:
		AnimatedTransform(const Transform& trans0, float t0, const Transform& trans1, float t1);

		void Interpolate(float t, Transform& trans) const;
		bool HasScale() const;

		static void Decompose(const Matrix44f& m, Vector3f& T, Quaternion& R, Matrix44f& S);
	private:
		// start transform
		const float t0_;
		const Transform& trans0_;
		// end transform
		const float t1_;
		const Transform& trans1_;
		// has animated?
		const bool hasAnimated_;
		// decompose transform 
		// translation
		Vector3f T_[2];
		// rotation
		Quaternion R_[2];
		// scale
		Matrix44f S_[2];
	};

}


#endif
