/********************************************************************
	created:	2016/07/30
	created:	30:7:2016   23:42
	filename: 	D:\Code\TinyRay\TinyRay\math\vector2.h
	file path:	D:\Code\TinyRay\TinyRay\math
	file base:	vector2
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	Vector2
*********************************************************************/
#ifndef __VECTOR2_H__
#define __VECTOR2_H__

#include "math_fwd.h"

namespace tinyray
{
	template <typename T>
	class Vector2
	{
	public:
		T x, y;

		Vector2() : x(T(0)), y(T(0)) {}
		Vector2(T _x, T _y) : x(_x), y(_y) {}
		Vector2(const Vector2& vec2) : x(vec2.x), y(vec2.y) {}
		Vector2& operator=(const Vector2& vec2) { x = vec2.x; y = vec2.y; return *this; }

		T& operator[](int i) { return (&x)[i]; }
		T operator[](int i) const { return (&x)[i]; }

		bool operator==(const Vector2& vec2) const { return equal_t(vec2.x, x) && equal_t(vec2.y, y); }
		bool operator!=(const Vector2& vec2) const { return !operator==(vec2); }

		Vector2 operator-() const { return Vector2(-x, -y); }

		Vector2 operator+(const Vector2& vec2) const { return Vector2(x + vec2.x, y + vec2.y); }
		Vector2 operator-(const Vector2& vec2) const { return Vector2(x - vec2.x, y - vec2.y); }

		Vector2& operator+=(const Vector2& vec2) { x += vec2.x; y += vec2.y; return *this; }
		Vector2& operator-=(const Vector2& vec2) { x -= vec2.x; y -= vec2.y; return *this; }

		template <typename U>
		Vector2& operator*=(U k) { x *= k; y *= k; return *this; }

		template <typename U>
		Vector2& operator/=(U k) { float f = 1.0f / k; x *= f; y *= f; return *this; }

		void Zero() { x = y = T(0); }
		void Normalize()
		{
			float sq = x * x + y * y;
			if (sq > 0.0f)
			{
				float f = 1.0f / sqrt(sq);
				x *= f;
				y *= f;
			}
		}
		bool IsNormalized() const { return equal_t(LengthSQ(), 1.0f); }
		float Length() const
		{
			return sqrt(x * x + y * y);
		}
		float LengthSQ() const
		{
			return x * x + y * y;
		}
		Vector2 Interpolate(const Vector2& v1, float t) const
		{
			return *this + (v1 - *this) * t;
		}
	};

	template <typename T, typename U>
	inline Vector2<T> operator*(const Vector2<T>& vec2, U k)
	{
		return Vector2<T>(vec2.x * k, vec2.y * k);
	}

	template <typename T, typename U>
	inline Vector2<T> operator*(U k, const Vector2<T>& vec2)
	{
		return Vector2<T>(vec2.x * k, vec2.y * k);
	}

	template <typename T, typename U>
	inline Vector2<T> operator/(const Vector2<T>& vec2, U k)
	{
		return Vector2<T>(vec2.x / k, vec2.y / k);
	}

	template <typename T>
	inline T DotProduct(const Vector2<T>& a, const Vector2<T>& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	template <typename T>
	inline float DistanceSQ(const Vector2<T>& a, const Vector2<T>& b)
	{
		float dx = a.x - b.x;
		float dy = a.y - b.y;
		return dx * dx + dy * dy;
	}

	template <typename T>
	inline float Distance(const Vector2<T>& a, const Vector2<T>& b)
	{
		return sqrt(DistanceSQ(a, b));
	}

	template <typename T>
	inline Vector2<T> Projection(const Vector2<T>& v, const Vector2<T>& n)
	{
		T dotp = DotProduct(v, n);
		T disSq = n.LengthSQ();
		return n * ((float)dotp / disSq);
	}

	template <typename T>
	inline float VectorAngle(const Vector2<T>& a, const Vector2<T>& b)
	{
		T dotp = DotProduct(a, b);
		float lenA = a.Length();
		float lenB = b.Length();
		return float(acos(dotp / (lenA * lenB)) * 180 / M_PI);
	}
}

#endif