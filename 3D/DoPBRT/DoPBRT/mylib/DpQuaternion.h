/********************************************************************
	created:	2014/06/21
	created:	21:6:2014   23:32
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpQuaternion.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpQuaternion
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Quaternion
*********************************************************************/

#ifndef __DP_QUATERNION__
#define __DP_QUATERNION__

#include "DpMath.h"
#include "DpVector3.h"
#include "DpMatrix43.h"
#include "DpMatrix44.h"

namespace dopixel
{
	namespace math
	{
		class Quaternion
		{
		public:
			float x, y, z, w;

			Quaternion() : x(0), y(0), z(0), w(1) {}
			Quaternion(float _x, float _y, float _z, float _w)
				: x(_x), y(_y), z(_z), w(_w) {}
			Quaternion(const Quaternion& q)
				: x(q.x), y(q.y), z(q.z), w(q.w) {}
			Quaternion(const float* q) { memcpy(this, q, sizeof(float) * 4); }

			bool operator==(const Quaternion& q) const { return equal_t(x, q.x) && equal_t(y, q.y) && equal_t(z, q.z) && equal_t(w, q.w); }
			bool operator!=(const Quaternion& q) const { return ! operator==(q); }

			Quaternion& operator+=(const Quaternion& q) { x += q.x; y += q.y; z += q.z; w += q.w; return *this; }
			Quaternion operator+(const Quaternion& q) const { Quaternion _q = *this; _q += q; return _q; }
			
			Quaternion& operator-=(const Quaternion& q) { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return *this; }
			Quaternion operator-(const Quaternion& q) const { Quaternion _q = *this; _q -= q; return _q; }

			Quaternion operator-() const { return Quaternion(-x, -y, -z, -w); }

			Quaternion& operator*=(float k) { x *= k; y *= k; z *= k; w *= k; return *this; }
			Quaternion operator*(float k) const { Quaternion q = *this; q *= k; return q; }

			Quaternion& operator/=(float k) { float f = 1.0f / k; x *= f; y *= f; z *= f; w *= f; return *this; }
			Quaternion operator/(float k) const { Quaternion q = *this; q /= k; return q; }

			//  [w1 v1] [w2 v2]
			// =[w1w2 - v1.v2   w1v2 + w2v1 + v1xv2]
			Quaternion operator*(const Quaternion& q) const
			{
				Quaternion _q;

				_q.w = w * q.w - x * q.x - y * q.y - z * q.z;
				_q.x = w * q.x + x * q.w + z * q.y - y * q.z;
				_q.y = w * q.y + y * q.w + x * q.z - z * q.x;
				_q.z = w * q.z + z * q.w + y * q.x - x * q.y;
				return _q;
			}

			Quaternion& operator*=(const Quaternion& q)
			{
				*this = *this * q;
				return *this;
			}

			float Length() const
			{
				return sqrt(x * x + y * y + z * z + w * w);
			}
			float LengthSQ() const
			{
				return x * x + y * y + z * z + w * w;
			}

			void Identity()
			{
				w = 1.0f;
				x = y = z = 0.0f;
			}

			void Normalize()
			{
				float len = Length();
				if (len > 0.0f)
				{
					float f = 1.0f / len;
					x *= f;
					y *= f;
					z *= f;
					w *= f;
				}
				else
				{
					Identity();
				}
			}
			bool IsNormalized() const { return equal_t(LengthSQ(), 1.0f); }

			void ToAxisAngle(Vector3f& v, float& angle) const
			{
				float w2radian = 0.0f;
				if (w <= 1.0f)
					w2radian = (float)M_PI;
				else if (w >= 1.0f)
					w2radian = 0.0f;
				else
					w2radian = acos(w);

				angle = float(radian2angle(w2radian * 2.0f));

				// [w x y z] = [cos(angle / 2) sin(angle / 2)v]
				// w = cos(angle / 2)  sin(angle /2)^2 + cos(angle /2)^2 = 1;
				
				float s = 1.0f - w * w;
				if (s <= 0.0f)
					v = Vector3f(1.0f, 0.0f, 0.0f);
				else
				{
					float t = 1.0f / sqrt(s);
					v = Vector3f(x * t, y * t, z * t);
				}
			}
		};

		inline float QuaternionDot(const Quaternion& q, const Quaternion& _q)
		{
			return q.w * _q.w + q.x * _q.x + q.y * _q.y + q.z * _q.z;
		}

		inline Quaternion QuaternionConjugate(const Quaternion& q)
		{
			return Quaternion(-q.x, -q.y, -q.z, q.w);
		}

		inline Quaternion QuaternionInverse(const Quaternion& q)
		{
			Quaternion _q = QuaternionConjugate(q);
			_q /= _q.Length();
			return _q;
		}

		// if q = (cos(theta), sin(theta) * v); ln(q) = (0, theta * v)
		inline Quaternion QuaternionLn(const Quaternion& q)
		{
			Vector3f v;
			float theta = 0.0f;
			q.ToAxisAngle(v, theta);
			float alpha = theta / 2.0f;
			return Quaternion(alpha * v.x, alpha * v.y, alpha * v.z, 0);
		}

		inline Quaternion QuaternionPow(const Quaternion& q, float exp)
		{
			if (abs(q.w) > 0.9999f)
				return q;

			float alpha = acos(q.w);
			float theta = alpha * exp;
			float f = sin(theta) / sin(alpha);

			Quaternion _q;
			_q.w = cos(theta);
			_q.x = q.x * f;
			_q.y = q.y * f;
			_q.z = q.z * f;

			return _q;
		}

		// Spherical linear interpolation between Q0 (t == 0) and Q1 (t == 1).
		// Expects unit quaternions.
		// Slerp(q0, q1, t) = (sin(1 - t)w / sinw)q0 + (sintw / sinw)q1  cosw = q0 . q1 / |q0||q1|
		inline Quaternion QuaternionSlerp(const Quaternion& q0, const Quaternion& q1, float t)
		{
			if (t <= 0.0f)
				return q0;
			if (t >= 1.0f)
				return q1;

			float cosTheta = QuaternionDot(q0, q1);
			if (cosTheta > .9995f)
			{
				Quaternion q = q0 * (1.0f - t) + q1 * t;
				q.Normalize();
				return q;
			}
			else
			{
				float theta = acosf(clamp_t(cosTheta, -1.0f, 1.0f));
				float thetap = theta * t;
				Quaternion qperp = q1 - q0 * cosTheta;
				qperp.Normalize();
				return q0 * cosf(thetap) + qperp * sinf(thetap);
			}
		}

		// v must be normal Vector
		inline Quaternion QuaternionRotationAxis(const Vector3f& v, float radian)
		{
			assert(equal_t(v.LengthSQ(), 1.0f));

			float thetaOver2 = radian * 0.5f;
			float sinThetaOver2 = sin(thetaOver2);

			Quaternion q;
			q.w = cos(thetaOver2);
			q.x = v.x * sinThetaOver2;
			q.y = v.y * sinThetaOver2;
			q.z = v.z * sinThetaOver2;
			return q;
		}

		inline Quaternion QuaternionRotationX(float radian)
		{
			float thetaOver2 = radian * 0.5f;

			Quaternion q;
			q.w = cos(thetaOver2);
			q.x = sin(thetaOver2);
			q.y = 0.0f;
			q.z = 0.0f;
			return q;
		}

		inline Quaternion QuaternionRotationY(float radian)
		{
			float thetaOver2 = radian * 0.5f;

			Quaternion q;
			q.w = cos(thetaOver2);
			q.x = 0.0f;
			q.y = sin(thetaOver2);
			q.z = 0.0f;
			return q;
		}

		inline Quaternion QuaternionRotationZ(float radian)
		{
			float thetaOver2 = radian * 0.5f;

			Quaternion q;
			q.w = cos(thetaOver2);
			q.x = 0.0f;
			q.y = 0.0f;
			q.z = sin(thetaOver2);
			return q;
		}

		inline Quaternion operator*(const Quaternion& q, const Vector3f& v)
		{
			Quaternion p(v.x, v.y, v.z, 0);
			return q * p;
		}

		inline Vector3f QuaternionRotateVector(const Vector3f& v, const Vector3f& axis, float radian)
		{
			Vector3f _v;

			Quaternion q = QuaternionRotationAxis(axis, radian);
			Quaternion qInverse = QuaternionInverse(q);
			Quaternion p = q * v * qInverse;
			_v.x = p.x;
			_v.y = p.y;
			_v.z = p.z;
			return _v;
		}

		// To Quaternion
		template <typename T>
		Quaternion QuaternionFromMatrix(const Matrix43<T>& n)
		{
			Quaternion q;
			T trace = n.m[0][0] + n.m[1][1] + n.m[2][2];
			if (trace > 0.f) {
				// Compute w from matrix trace, then xyz
				// 4w^2 = m[0][0] + m[1][1] + m[2][2] + m[3][3] (but m[3][3] == 1)
				float s = sqrtf(trace + 1.0);
				q.w = s / 2.0f;
				s = 0.5f / s;
				q.x = (n.m[2][1] - n.m[1][2]) * s;
				q.y = (n.m[0][2] - n.m[2][0]) * s;
				q.z = (n.m[1][0] - n.m[0][1]) * s;
			}
			else {
				// Compute largest of $x$, $y$, or $z$, then remaining components
				const int nxt[3] = { 1, 2, 0 };
				T v[3];
				int i = 0;
				if (n.m[1][1] > n.m[0][0]) i = 1;
				if (n.m[2][2] > n.m[i][i]) i = 2;
				int j = nxt[i];
				int k = nxt[j];
				float s = sqrtf((n.m[i][i] - (n.m[j][j] + n.m[k][k])) + 1.0);
				v[i] = s * 0.5f;
				if (s != 0.f) s = 0.5f / s;
				q.w  = (n.m[k][j] - n.m[j][k]) * s;
				v[j] = (n.m[j][i] + n.m[i][j]) * s;
				v[k] = (n.m[k][i] + n.m[i][k]) * s;
				q.x = v[0];
				q.y = v[1];
				q.z = v[2];
			}
			return q;
		}

		template <typename T>
		Quaternion QuaternionFromMatrix(const Matrix44<T>& n)
		{
			Quaternion q;
			T trace = n.m[0][0] + n.m[1][1] + n.m[2][2];
			if (trace > 0.f) {
				// Compute w from matrix trace, then xyz
				// 4w^2 = m[0][0] + m[1][1] + m[2][2] + m[3][3] (but m[3][3] == 1)
				float s = sqrtf(trace + 1.0);
				q.w = s / 2.0f;
				s = 0.5f / s;
				q.x = (n.m[2][1] - n.m[1][2]) * s;
				q.y = (n.m[0][2] - n.m[2][0]) * s;
				q.z = (n.m[1][0] - n.m[0][1]) * s;
			}
			else {
				// Compute largest of $x$, $y$, or $z$, then remaining components
				const int nxt[3] = { 1, 2, 0 };
				T v[3];
				int i = 0;
				if (n.m[1][1] > n.m[0][0]) i = 1;
				if (n.m[2][2] > n.m[i][i]) i = 2;
				int j = nxt[i];
				int k = nxt[j];
				float s = sqrtf((n.m[i][i] - (n.m[j][j] + n.m[k][k])) + 1.0);
				v[i] = s * 0.5f;
				if (s != 0.f) s = 0.5f / s;
				q.w = (n.m[k][j] - n.m[j][k]) * s;
				v[j] = (n.m[j][i] + n.m[i][j]) * s;
				v[k] = (n.m[k][i] + n.m[i][k]) * s;
				q.x = v[0];
				q.y = v[1];
				q.z = v[2];
			}
			return q;
		}

		// To Matrix
		template <typename T>
		inline Matrix43<T>& MatrixFromQuaternion(Matrix43<T>& n, const Quaternion& q)
		{
			n.ZeroTranslation();

			float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
			float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
			float wx = q.x * q.w, wy = q.y * q.w, wz = q.z * q.w;

			n.m[0][0] = 1.0f - 2.0f * (yy + zz);
			n.m[0][1] = 2.0f * (xy + wz);
			n.m[0][2] = 2.0f * (xz - wy);
			n.m[1][0] = 2.0f * (xy - wz);
			n.m[1][1] = 1.0f - 2.0f * (xx + zz);
			n.m[1][2] = 2.0f * (yz + wx);
			n.m[2][0] = 2.0f * (xz + wy);
			n.m[2][1] = 2.0f * (yz - wx);
			n.m[2][2] = 1.0f - 2.0f * (xx + yy);

			return n;
		}

		template <typename T>
		inline Matrix44<T>& MatrixFromQuaternion(Matrix44<T>& n, const Quaternion& q)
		{
			n.ZeroTranslation();

			float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
			float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
			float wx = q.x * q.w, wy = q.y * q.w, wz = q.z * q.w;

			n.m[0][0] = 1.0f - 2.0f * (yy + zz);
			n.m[0][1] = 2.0f * (xy + wz);
			n.m[0][2] = 2.0f * (xz - wy);
			n.m[1][0] = 2.0f * (xy - wz);
			n.m[1][1] = 1.0f - 2.0f * (xx + zz);
			n.m[1][2] = 2.0f * (yz + wx);
			n.m[2][0] = 2.0f * (xz + wy);
			n.m[2][1] = 2.0f * (yz - wx);
			n.m[2][2] = 1.0f - 2.0f * (xx + yy);

			n.m[0][3] = 0.0f;
			n.m[1][3] = 0.0f;
			n.m[2][3] = 0.0f;
			n.m[3][3] = 1.0f;

			return n;
		}

		template <typename OS>
		inline OS& operator << (OS& os, const Quaternion& q)
		{
			os << "(" << q.x << "," << q.y << "," << q.z << "," << q.w << ")";
			return os;
		}
	}
}

#endif