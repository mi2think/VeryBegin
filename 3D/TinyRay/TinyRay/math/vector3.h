/********************************************************************
	created:	2016/07/06
	created:	6:7:2016   22:28
	filename: 	D:\Code\TinyRay\TinyRay\math\vector3.h
	file path:	D:\Code\TinyRay\TinyRay\math
	file base:	vector3
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Vector3
*********************************************************************/
#ifndef __VECTOR3_H__
#define __VECTOR3_H__

#include "math_fwd.h"

namespace tinyray
{
	template <typename T>
	class Vector3
	{
	public:
		T x, y, z;

		Vector3() : x(T(0)), y(T(0)), z(T(0)) {}
		Vector3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
		Vector3(const Vector3& vec3) : x(vec3.x), y(vec3.y), z(vec3.z) {}
		Vector3& operator=(const Vector3& vec3) { x = vec3.x; y = vec3.y; z = vec3.z; return *this; }

		T& operator[](int i) { return (&x)[i]; }
		T operator[](int i) const { return (&x)[i]; }

		bool operator==(const Vector3& vec3) const { return equal_t(vec3.x, x) && equal_t(vec3.y, y) && equal_t(vec3.z, z); }
		bool operator!=(const Vector3& vec3) const { return !operator==(vec3); }

		Vector3 operator-() const { return Vector3(-x, -y, -z); }

		Vector3 operator+(const Vector3& vec3) const { return Vector3(x + vec3.x, y + vec3.y, z + vec3.z); }
		Vector3 operator-(const Vector3& vec3) const { return Vector3(x - vec3.x, y - vec3.y, z - vec3.z); }
		Vector3 operator*(const Vector3& vec3) const { return Vector3(x * vec3.x, y * vec3.y, z * vec3.z); }

		Vector3& operator+=(const Vector3& vec3) { x += vec3.x; y += vec3.y; z += vec3.z; return *this; }
		Vector3& operator-=(const Vector3& vec3) { x -= vec3.x; y -= vec3.y; z -= vec3.z; return *this; }
		Vector3& operator*=(const Vector3& vec3) { x *= vec3.x; y *= vec3.y; z *= vec3.z; return *this; }

		template <typename U>
		Vector3& operator*=(U k) { x *= k; y *= k; z *= k; return *this; }

		template <typename U>
		Vector3& operator/=(U k) { float f = 1.0f / k; x *= f; y *= f; z *= f; return *this; }

		void Zero() { x = y = z = T(0); }
		void Normalize()
		{
			float sq = x * x + y * y + z * z;
			if (sq > 0.0f)
			{
				float f = 1.0f / sqrt(sq);
				x *= f;
				y *= f;
				z *= f;
			}
		}
		bool IsNormalized() const { return equal_t(LengthSQ(), 1.0f); }
		float Length() const
		{
			return sqrt(x * x + y * y + z * z);
		}
		float LengthSQ() const
		{
			return x * x + y * y + z * z;
		}
	};

	template <typename T, typename U>
	inline Vector3<T> operator*(const Vector3<T>& vec3, U k)
	{
		return Vector3<T>(vec3.x * k, vec3.y * k, vec3.z * k);
	}

	template <typename T, typename U>
	inline Vector3<T> operator*(U k, const Vector3<T>& vec3)
	{
		return Vector3<T>(vec3.x * k, vec3.y * k, vec3.z * k);
	}

	template <typename T, typename U>
	inline Vector3<T> operator/(const Vector3<T>& vec3, U k)
	{
		return Vector3<T>(vec3.x / k, vec3.y / k, vec3.z / k);
	}

	template <typename T>
	inline T DotProduct(const Vector3<T>& a, const Vector3<T>& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	template <typename T>
	inline Vector3<T> CrossProduct(const Vector3<T>& a, const Vector3<T>& b)
	{
		return Vector3<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}

	template <typename T>
	inline float DistanceSQ(const Vector3<T>& a, const Vector3<T>& b)
	{
		float dx = a.x - b.x;
		float dy = a.y - b.y;
		float dz = a.z - b.z;
		return dx * dx + dy * dy + dz * dz;
	}

	template <typename T>
	inline float Distance(const Vector3<T>& a, const Vector3<T>& b)
	{
		return sqrt(DistanceSQ(a, b));
	}

	template <typename T>
	inline Vector3<T> Projection(const Vector3<T>& v, const Vector3<T>& n)
	{
		T dotp = DotProduct(v, n);
		T disSq = n.LengthSQ();
		return n * ((float)dotp / disSq);
	}

	template <typename T>
	inline float VectorAngle(const Vector3<T>& a, const Vector3<T>& b)
	{
		T dotp = DotProduct(a, b);
		float lenA = a.Length();
		float lenB = b.Length();
		return float(acos(dotp / (lenA * lenB)) * 180 / M_PI);
	}
}

#endif
