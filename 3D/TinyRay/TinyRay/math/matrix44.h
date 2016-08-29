/********************************************************************
	created:	2016/08/08
	created:	8:8:2016   10:58
	filename: 	D:\Code\TinyRay\TinyRay\math\matrix44.h
	file path:	D:\Code\TinyRay\TinyRay\math
	file base:	matrix44
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Matrix44
*********************************************************************/
#ifndef __MATRIX44_H__
#define __MATRIX44_H__

#include "math_fwd.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix33.h"

namespace tinyray
{
	template <typename T>
	class Matrix44
	{
	public:
		enum { R = 4, C = 4, N = R * C };

		union
		{
			struct
			{
				T m11, m12, m13, m14;
				T m21, m22, m23, m24;
				T m31, m32, m33, m34;
				T m41, m42, m43, m44;
			};
			T m[R][C];
		};

		Matrix44() {}
		Matrix44(const T* _m) { memcpy(m, _m, sizeof(T) * N); }
		Matrix44(const Matrix44& _m) { memcpy(m, _m.m, sizeof(T) * N); }
		Matrix44(T _m11, T _m12, T _m13, T _m14,
			T _m21, T _m22, T _m23, T _m24,
			T _m31, T _m32, T _m33, T _m34,
			T _m41, T _m42, T _m43, T _m44)
			: m11(_m11), m12(_m12), m13(_m13), m14(_m14)
			, m21(_m21), m22(_m22), m23(_m23), m24(_m24)
			, m31(_m31), m32(_m32), m33(_m33), m34(_m34)
			, m41(_m41), m42(_m42), m43(_m43), m44(_m44) {}

		Matrix44& operator=(const Matrix44& _m)
		{
			memcpy(m, _m.m, sizeof(T) * N);
			return *this;
		}

		bool operator==(const Matrix44& _m) const
		{
			for (int i = 0; i < R; ++i)
			{
				for (int j = 0; j < C; ++j)
				{
					if (!equal_t(m[i][j], _m.m[i][j]))
						return false;
				}
			}
			return true;
		}
		bool operator!=(const Matrix44& _m) const { return !operator==(_m); }

		Matrix44& operator+=(const Matrix44& _m)
		{
			for (int i = 0; i < R; ++i)
			{
				for (int j = 0; j < C; ++j)
				{
					m[i][j] += _m.m[i][j];
				}
			}
			return *this;
		}

		Matrix44 operator+(const Matrix44& _m) const
		{
			Matrix44 n = *this;
			n += _m;
			return n;
		}

		Matrix44& operator-=(const Matrix44& _m)
		{
			for (int i = 0; i < R; ++i)
			{
				for (int j = 0; j < C; ++j)
				{
					m[i][j] -= _m.m[i][j];
				}
			}
			return *this;
		}

		Matrix44 operator-(const Matrix44& _m) const
		{
			Matrix44 n = *this;
			n -= _m;
			return n;
		}

		template <typename U>
		Matrix44& operator*=(U k)
		{
			for (int i = 0; i < R; ++i)
			{
				for (int j = 0; j < C; ++j)
				{
					m[i][j] *= k;
				}
			}
			return *this;
		}
		template <typename U>
		Matrix44 operator*(U k) const
		{
			Matrix44 n = *this;
			n *= k;
			return n;
		}

		template <typename U>
		Matrix44& operator/=(U k)
		{
			float f = 1.0f / k;
			return operator*=(f);
		}
		template <typename U>
		Matrix44 operator/(U k) const
		{
			Matrix44 n = *this;
			n /= k;
			return n;
		}

		Matrix44 operator-() const
		{
			Matrix44 n;
			for (int i = 0; i < R; ++i)
			{
				for (int j = 0; j < C; ++j)
				{
					n.m[i][j] = -m[i][j];
				}
			}
			return n;
		}

		Matrix44 operator*(const Matrix44& _m) const
		{
			Matrix44 n;
			for (int i = 0; i < R; ++i)
			{
				for (int j = 0; j < C; ++j)
				{
					T sum{ 0 };
					for (int k = 0; k < C; ++k)
					{
						sum += m[i][k] * _m.m[k][j];
					}
					n.m[i][j] = sum;
				}
			}
			return n;
		}

		Matrix44 operator*=(const Matrix44& _m)
		{
			*this = *this * _m;
			return *this;
		}

		void Identity()
		{
			for (int i = 0; i < R; ++i)
			{
				for (int j = 0; j < C; ++j)
				{
					i == j ? m[i][j] = 1 : m[i][j] = 0;
				}
			}
		}

		// Translation methods
		void ZeroTranslation()
		{
			m41 = m42 = m43 = 0;
		}

		void SetTranslation(const Vector3<T>& v)
		{
			m41 = v.x;
			m42 = v.y;
			m43 = v.z;
		}

		Vector3<T> GetTranslation() const
		{
			return Vector3<T>(m41, m42, m43);
		}

		// Transpose
		void Transpose()
		{
			for (int i = 0; i < R; ++i)
			{
				for (int j = i; j < C; ++j)
				{
					if (i != j)
					{
						swap_t(m[i][j], m[j][i]);
					}
				}
			}
		}
	};

	template <typename T, typename U>
	inline Matrix44<T> operator*(U k, const Matrix44<T>& _m)
	{
		Matrix44<T> n = _m;
		n *= k;
		return n;
	}

	// Matrix n and _m must not be same one
	template <typename T>
	inline Matrix44<T>& MatrixTranspose(Matrix44<T>& n, const Matrix44<T>& _m)
	{
		assert(&n != &_m);

		for (int i = 0; i < Matrix44<T>::R; ++i)
		{
			for (int j = 0; j < Matrix44<T>::C; ++j)
			{
				n.m[i][j] = _m.m[j][i];
			}
		}

		return n;
	}

	// algebraic cofactor
	template <typename T>
	inline Matrix44<T>& MatrixAlgebraicCofactor(Matrix44<T>& n, const Matrix44<T>& _m)
	{
		T t[16];
		int index = 0;
		bool negative = false;
		for (int i = 0; i < Matrix44<T>::R; ++i)
		{
			for (int j = 0; j < Matrix44<T>::C; ++j)
			{
				T t2[9];
				int index2 = 0;
				for (int i2 = 0; i2 < Matrix44<T>::R; ++i2)
				{
					for (int j2 = 0; j2 < Matrix44<T>::C; ++j2)
					{
						if (i2 == i || j2 == j)
							continue;
						t2[index2++] = _m.m[i2][j2];
					}
				}

				t[index] = MatrixDeterminant(Matrix33<T>(t2));
				if (negative)
				{
					t[index] = -t[index];
				}
				negative = !negative;
				++index;
			}
			negative = !negative;
		}
		n = Matrix44<T>(t);
		return n;
	}

	// standard adjugate
	template <typename T>
	inline Matrix44<T>& MatrixStandardAdjugate(Matrix44<T>& n, const Matrix44<T>& _m)
	{
		Matrix44<T> t;
		MatrixAlgebraicCofactor(t, _m);
		MatrixTranspose(n, t);
		return n;
	}

	template <typename T>
	inline float MatrixDeterminant(const Matrix44<T>& _m)
	{
		//Matrix44<T> t;
		//MatrixAlgebraicCofactor(t, _m);
		//return _m.m11 * t.m11 + _m.m12 * t.m12 + _m.m13 * t.m13 + _m.m14 * t.m14;

		// for improve precision
		auto& m = _m.m;
		return  m[0][0] * (
			m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
			m[2][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) +
			m[3][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2])
			) -
			m[1][0] * (
				m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) -
				m[2][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
				m[3][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2])
				) +
			m[2][0] * (
				m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) -
				m[1][1] * (m[0][2] * m[3][3] - m[0][3] * m[3][2]) +
				m[3][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2])
				) -
			m[3][0] * (
				m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) -
				m[1][1] * (m[0][2] * m[2][3] - m[0][3] * m[2][2]) +
				m[2][1] * (m[0][2] * m[1][3] - m[0][3] * m[1][2])
				);
	}

	template <typename T>
	inline Matrix44<T>& MatrixInverse(Matrix44<T>& n, const Matrix44<T>& _m)
	{
		MatrixStandardAdjugate(n, _m);
		float det = _m.m11 * n.m11 + _m.m12 * n.m21 + _m.m13 * n.m31 + _m.m14 * n.m41;
		n /= det;
		return n;
	}

	template <typename T>
	inline Matrix44<T>& MatrixMultiply(Matrix44<T>& n, const Matrix44<T>& m1, const Matrix44<T>& m2)
	{
		Matrix44 n;
		for (int i = 0; i < Matrix44<T>::R; ++i)
		{
			for (int j = 0; j < Matrix44<T>::C; ++j)
			{
				T sum{ 0 };
				for (int k = 0; k < Matrix44<T>::C; ++k)
				{
					sum += m1.m[i][k] * m2.m[k][j];
				}
				n.m[i][j] = sum;
			}
		}
		return n;
	}

	// Translation

	template <typename T>
	inline Matrix44<T>& MaxtrixTranslation(Matrix44<T>& n, const Vector3<T>& v)
	{
		for (int i = 0; i < Matrix44<T>::R; ++i)
		{
			for (int j = 0; j < Matrix44<T>::C; ++j)
			{
				i == j ? n.m[i][j] = 1 : n.m[i][j] = 0;
			}
		}

		n.m41 = v.x;
		n.m42 = v.y;
		n.m43 = v.z;

		return n;
	}

	// Scale

	// scale by coordinate axis
	template <typename T>
	inline Matrix44<T>& MatrixScaling(Matrix44<T>& n, const Vector3<T>& v)
	{
		n.m11 = v.x;  n.m12 = 0.0f; n.m13 = 0.0f; n.m14 = 0.0f;
		n.m21 = 0.0f; n.m22 = v.y;  n.m23 = 0.0f; n.m24 = 0.0f;
		n.m31 = 0.0f; n.m32 = 0.0f; n.m33 = v.z;  n.m34 = 0.0f;
		n.m41 = 0.0f; n.m42 = 0.0f;	n.m43 = 0.0f; n.m44 = 1.0f;
		return n;
	}

	// Rotate

	template <typename T>
	inline Matrix44<T>& MatrixRotationX(Matrix44<T>& n, float radian)
	{
		float s = 0.0f;
		float c = 0.0f;
		sincos(s, c, radian);

		n.m11 = 1.0f; n.m12 = 0.0f; n.m13 = 0.0f; n.m14 = 0.0f;
		n.m21 = 0.0f; n.m22 = c;	n.m23 = s;	  n.m24 = 0.0f;
		n.m31 = 0.0f; n.m32 = -s;	n.m33 = c;	  n.m34 = 0.0f;
		n.m41 = 0.0f; n.m42 = 0.0f;	n.m43 = 0.0f; n.m44 = 1.0f;

		return n;
	}

	template <typename T>
	inline Matrix44<T>& MatrixRotationY(Matrix44<T>& n, float radian)
	{
		float s = 0.0f;
		float c = 0.0f;
		sincos(s, c, radian);

		n.m11 = c;    n.m12 = 0.0f; n.m13 = -s;   n.m14 = 0.0f;
		n.m21 = 0.0f; n.m22 = 1.0f;	n.m23 = 0;	  n.m24 = 0.0f;
		n.m31 = s;	  n.m32 = 0.0f;	n.m33 = c;	  n.m34 = 0.0f;
		n.m41 = 0.0f; n.m42 = 0.0f;	n.m43 = 0.0f; n.m44 = 1.0f;

		return n;
	}

	template <typename T>
	inline Matrix44<T>& MatrixRotationZ(Matrix44<T>& n, float radian)
	{
		float s = 0.0f;
		float c = 0.0f;
		sincos(s, c, radian);

		n.m11 = c;    n.m12 = s;    n.m13 = 0.0f; n.m14 = 0.0f;
		n.m21 = -s;   n.m22 = c;	n.m23 = 0.0f; n.m24 = 0.0f;
		n.m31 = 0.0f; n.m32 = 0.0f;	n.m33 = 1.0f; n.m34 = 0.0f;
		n.m41 = 0.0f; n.m42 = 0.0f;	n.m43 = 0.0f; n.m44 = 1.0f;

		return n;
	}

	template <typename T>
	inline Matrix44<T>& MatrixRotationXYZ(Matrix44<T>& m, float radianX, float radianY, float radianZ)
	{
		Matrix44<T> mx;
		MatrixRotationX(mx, radianX);

		Matrix44<T> my;
		MatrixRotationY(my, radianY);

		Matrix44<T> mz;
		MatrixRotationZ(mz, radianZ);

		Matrix44<T> n;
		MatrixMultiply(n, mx, my);

		return MatrixMultiply(m, n, mz);
	}

	template <typename T>
	inline Matrix44<T>& MatrixRotationZYX(Matrix44<T>& m, float radianX, float radianY, float radianZ)
	{
		Matrix44<T> mx;
		MatrixRotationX(mx, radianX);

		Matrix44<T> my;
		MatrixRotationY(my, radianY);

		Matrix44<T> mz;
		MatrixRotationZ(mz, radianZ);

		Matrix44<T> n;
		MatrixMultiply(n, mz, my);

		return MatrixMultiply(m, n, mx);
	}

	// Assume v through the origin, v must be normal Vector
	template <typename T>
	inline Matrix44<T>& MatrixRotationAxis(Matrix44<T>& n, const Vector4<T>& v, float radian)
	{
		assert(v.IsNormalized());

		float s = 0.0f;
		float c = 0.0f;
		sincos(s, c, radian);

		// Common expr 
		float a = 1.0f - c;
		float ax = a * v.x;
		float ay = a * v.y;
		float az = a * v.z;

		n.ZeroTranslation();

		n.m11 = ax * v.x + c;
		n.m12 = ax * v.y + v.z * s;
		n.m13 = ax * v.z - v.y * s;
		n.m14 = 0.0f;

		n.m21 = ay * v.x - az * s;
		n.m22 = ay * v.y + c;
		n.m23 = ay * v.z + v.x * s;
		n.m24 = 0.0f;

		n.m31 = az * v.x + ay * s;
		n.m32 = az * v.y - ax * s;
		n.m33 = az * v.z + c;
		n.m34 = 0.0f;

		n.m44 = 1.0f;

		return n;
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename T, typename U>
	inline Vector4<T> operator*(const Vector4<T>& v, const Matrix44<U>& _m)
	{
		return Vector4<T>(v.x * _m.m11 + v.y * _m.m21 + v.z * _m.m31 + v.w * _m.m41,
			v.x * _m.m12 + v.y * _m.m22 + v.z * _m.m32 + v.w * _m.m42,
			v.x * _m.m13 + v.y * _m.m23 + v.z * _m.m33 + v.w * _m.m43,
			v.x * _m.m14 + v.y * _m.m24 + v.z * _m.m34 + v.w * _m.m44);
	}

	template <typename T, typename U>
	inline Vector4<T>& operator*=(Vector4<T>& v, const Matrix44<U>& _m)
	{
		v = v * _m;
		return v;
	}

	template <typename T, typename U>
	inline Vector3<T> operator*(const Vector3<T>& v, const Matrix44<U>& _m)
	{
		Vector4<T> v4(v.x, v.y, v.z, 1.0f);
		v4 *= _m;
		return v4.DivW();
	}

	template <typename T, typename U>
	inline Vector3<T> TransformVec3(const Vector3<T>& v, const Matrix44<U>& _m)
	{
		Vector4<T> v4(v.x, v.y, v.z, 0.0f);
		v4 *= _m;
		return v4.DivW();
	}
}

#endif