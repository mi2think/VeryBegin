/********************************************************************
	created:	2016/06/01
	created:	1:6:2016   11:37
	filename: 	D:\OneDrive\3D\DoPBRT\DoPBRT\core\transform.cpp
	file path:	D:\OneDrive\3D\DoPBRT\DoPBRT\core
	file base:	transform
	file ext:	cpp
	copyright:
		The copyright of this file is same with origin file in pbrt.
		You can find in the depot:https://github.com/mmp/pbrt-v2.
	modify:
	
	purpose:	
*********************************************************************/
#include "transform.h"

namespace dopbrt
{
	bool Transform::HasScale() const
	{
		float x = ApplyPoint(Point(1, 0, 0)).LengthSQ();
		float y = ApplyPoint(Point(0, 1, 0)).LengthSQ();
		float z = ApplyPoint(Point(0, 0, 1)).LengthSQ();
		return (!equal_t(x, 1)) || (!equal_t(y, 1)) || (!equal_t(z, 1));
	}

	Point Transform::ApplyPoint(const Point& pt) const
	{
		Vector4f v(pt.x, pt.y, pt.z, 1);
		v *= m_;
		assert(!equal_t(v.w, 0.0f));
		if (equal_t(v.w, 1.0f))
			return Point(v.x, v.y, v.z);
		else
			return Point(v.x, v.y, v.z) / v.w;
	}

	void Transform::ApplyPoint(const Point& pt, Point& ptTrans) const
	{
		Vector4f v(pt.x, pt.y, pt.z, 1);
		v *= m_;
		assert(!equal_t(v.w, 0.0f));
		if (equal_t(v.w, 1.0f))
			ptTrans = Point(v.x, v.y, v.z);
		else
			ptTrans = Point(v.x, v.y, v.z) / v.w;
	}

	Vector3f Transform::ApplyVector(const Vector3f& v) const
	{
		Matrix33f n;
		m_.LTMatrix33(n);
		return Vector3f(v) * n;
	}

	void Transform::ApplyVector(const Vector3f& v, Vector3f& vTrans) const
	{
		Matrix33f n;
		m_.LTMatrix33(n);
		vTrans = v;
		vTrans *= n;
	}

	Normal Transform::ApplyNormal(const Normal& n) const
	{
		Matrix44f m;
		MatrixTranspose(m, mInv_);
		Matrix33f mt;
		m_.LTMatrix33(mt);
		return Normal(n) * mt;
	}

	void Transform::ApplyNormal(const Normal& n, Normal& nTrans) const
	{
		Matrix44f m;
		MatrixTranspose(m, mInv_);
		Matrix33f mt;
		m_.LTMatrix33(mt);
		nTrans = n;
		nTrans *= mt;
	}

	Ray Transform::ApplyRay(const Ray& r) const
	{
		Ray ray = r;
		ApplyPoint(r.origin_, ray.origin_);
		ApplyPoint(r.direction_, ray.direction_);
		return ray;
	}

	void Transform::ApplyRay(const Ray& r, Ray& rTrans) const
	{
		rTrans = r;
		ApplyPoint(r.origin_, rTrans.origin_);
		ApplyPoint(r.direction_, rTrans.direction_);
	}

	BBox Transform::ApplyBBox(const BBox& b) const
	{
		BBox box = b;
		box.Transform(m_);
		return box;
	}

	void Transform::ApplyBBox(const BBox& b, BBox& bTrans) const
	{
		bTrans = b;
		bTrans.Transform(m_);
	}

	RayDifferential Transform::ApplyRayDifferential(const RayDifferential& r) const
	{
		RayDifferential rTrans;
		ApplyRay(r, rTrans);
		rTrans.hasDifferentials_ = r.hasDifferentials_;
		ApplyPoint(r.rxOrigin_, rTrans.rxOrigin_);
		ApplyPoint(r.ryOrigin_, rTrans.ryOrigin_);
		ApplyVector(r.rxDirection_, rTrans.rxDirection_);
		ApplyVector(r.ryDirection_, rTrans.ryDirection_);
		return rTrans;
	}

	void Transform::ApplyRayDifferential(const RayDifferential& r, RayDifferential& rTrans) const
	{
		ApplyRay(r, rTrans);
		rTrans.hasDifferentials_ = r.hasDifferentials_;
		ApplyPoint(r.rxOrigin_, rTrans.rxOrigin_);
		ApplyPoint(r.ryOrigin_, rTrans.ryOrigin_);
		ApplyVector(r.rxDirection_, rTrans.rxDirection_);
		ApplyVector(r.ryDirection_, rTrans.ryDirection_);
	}

	Transform Transform::operator *(const Transform& t) const
	{
		Matrix44f m, mInv;
		MatrixMultiply(m, m_, t.m_);
		MatrixMultiply(mInv, mInv_, t.mInv_);
		return Transform(m, mInv);
	}

	bool Transform::SwapsHandedness() const
	{
		Matrix33f n;
		m_.LTMatrix33(n);
		return MatrixDeterminant(n) < 0;
	}

	Transform Translate(const Vector3f& delta)
	{
		Matrix44f m, mInv;
		MaxtrixTranslation(m, delta);
		MatrixTranspose(mInv, m);
		return Transform(m, mInv);
	}

	Transform Scale(float x, float y, float z)
	{
		Matrix44f m, mInv;
		MatrixScaling(m, Vector3f(x, y, z));
		MatrixTranspose(mInv, m);
		return Transform(m, mInv);
	}

	Transform RotateX(float angle)
	{
		Matrix44f m, mInv;
		MatrixRotationX(m, angle2radian(angle));
		MatrixTranspose(mInv, m);
		return Transform(m, mInv);
	}

	Transform RotateY(float angle)
	{
		Matrix44f m, mInv;
		MatrixRotationY(m, angle2radian(angle));
		MatrixTranspose(mInv, m);
		return Transform(m, mInv);
	}

	Transform RotateZ(float angle)
	{
		Matrix44f m, mInv;
		MatrixRotationZ(m, angle2radian(angle));
		MatrixTranspose(mInv, m);
		return Transform(m, mInv);
	}

	Transform Rotate(float angle, const Vector3f& axis)
	{
		Vector3f v = axis;
		v.Normalize();
		Matrix44f m, mInv;
		MatrixRotationAxis(m, Vector4f(v.x, v.y, v.z, 1.0f), angle2radian(angle));
		MatrixTranspose(mInv, m);
		return Transform(m, mInv);
	}

	Transform LookAt(const Point& pos, const Point& look, const Vector3f& up)
	{
		// n: target
		Vector3f n = look - pos;
		n.Normalize();
		// v: up
		Vector3f v = up;
		v.Normalize();
		// u: right
		// u = v x n
		Vector3f u = CrossProduct(v, n);
		// v = n x u
		v = CrossProduct(n, u);

		Matrix44f worldToCamera;
		auto& m = worldToCamera.m;
		m[0][0] = u.x;   m[0][1] = v.x;   m[0][2] = n.x;   m[0][3] = 0.0f;
		m[1][0] = u.y;   m[1][1] = v.y;   m[1][2] = n.y;   m[1][3] = 0.0f;
		m[2][0] = u.z;   m[2][1] = v.z;   m[2][2] = n.z;   m[2][3] = 0.0f;
		m[3][0] = -pos.x; m[3][1] = -pos.y; m[3][2] = -pos.z; m[3][3] = 1;
		Matrix44f mInv;
		MatrixTranspose(mInv, worldToCamera);
		return Transform(worldToCamera, mInv);
	}

	//////////////////////////////////////////////////////////////////////////

	AnimatedTransform::AnimatedTransform(const Transform& trans0, float t0, const Transform& trans1, float t1)
		: trans0_(trans0)
		, t0_(t0)
		, trans1_(trans1)
		, t1_(t1)
		, hasAnimated_(trans0 != trans1)
	{
		Decompose(trans0_.GetMatrix(), T_[0], R_[0], S_[0]);
		Decompose(trans1_.GetMatrix(), T_[1], R_[1], S_[1]);
	}
	
	void AnimatedTransform::Interpolate(float t, Transform& trans) const
	{
		if (!hasAnimated_ || t <= t0_)
		{
			trans = trans0_;
			return;
		}
		if (t >= t1_)
		{
			trans = trans1_;
			return;
		}

		float dt = (t - t0_) / (t1_ - t0_);

		// translation
		Vector3f v = (1.0f - dt) * T_[0] + dt * T_[1];

		// rotation
		Quaternion r = QuaternionSlerp(R_[0], R_[1], dt);

		// scale
		Matrix44f s;
		s.Identity();
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				s.m[i][j] = Lerp(dt, S_[0].m[i][j], S_[1].m[i][j]);
			}
		}

		// compute interpolated matrix as product of interpolated components
		Matrix44f rm;
		MatrixFromQuaternion(rm, r);
	
		Transform rt(rm);
		Transform tt = Translate(v);
		Transform st(s);
		trans = st * rt * tt;
	}

	bool AnimatedTransform::HasScale() const
	{
		return trans0_.HasScale() || trans1_.HasScale();
	}

	void AnimatedTransform::Decompose(const Matrix44f& m, Vector3f& T, Quaternion& R, Matrix44f& S)
	{
		// extract translation from transformation matrix
		T.x = m.m[3][0];
		T.y = m.m[3][1];
		T.z = m.m[3][2];

		// compute new transformation matrix without translation
		Matrix44f n = m;
		n.ZeroTranslation();
		n.m[0][3] = 0.0f; n.m[1][3] = 0.0f; n.m[2][3] = 0.0f; n.m[3][3] = 1.0f;

		// extract rotation
		float norm;
		int count = 0;
		Matrix44f r = n;
		do 
		{
			Matrix44f rnext;

			Matrix44f rit;
			MatrixTranspose(rnext, r);
			MatrixInverse(rit, rnext);

			for (int i = 0; i < Matrix44f::R; ++i)
			{
				for (int j = 0; j < Matrix44f::C; ++j)
				{
					rnext.m[i][j] = 0.5f * (r.m[i][j] + rit.m[i][j]);
				}
			}

			// compute norm of difference between r and rnext
			norm = 0.0f;
			for (int j = 0; j < 3; ++j)
			{
				float n = fabsf(r.m[0][j] - rnext.m[0][j]) +
					fabsf(r.m[1][j] - rnext.m[1][j]) +
					fabsf(r.m[2][j] - rnext.m[2][j]);
				norm = MAX(norm, n);
			}
			r = rnext;
		} while (++count < 100 && norm > 0.0001f);

		R = QuaternionFromMatrix(r);

		Matrix44f invr;
		MatrixInverse(invr, r);

		MatrixMultiply(S, n, invr);
	}
}