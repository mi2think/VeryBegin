#ifndef _COMMON_H
#define _COMMON_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <queue>
#include <set>
#include <algorithm>
#include <cstdarg>

#ifndef BYTE_DEF
#define BYTE_DEF
typedef unsigned char		BYTE;
#endif

#ifndef WORD_DEF
#define WORD_DEF
typedef unsigned short		WORD;
#endif

#ifndef DWORD_DEF
#define DWORD_DEF
typedef unsigned long		DWORD;
#endif

#ifndef QWORD_DEF
#define QWORD_DEF
typedef unsigned __int64	QWORD;
#endif


#define SAFE_DELETE(p)		do { if ((p)) delete (p); (p) = NULL; } while(0)
#define SAFE_DELETEARRY(p)	do { if ((p)) delete [] (p); (p) = NULL; } while(0)
#define SAFE_FREE(p)		do { if ((p)) free((p)); (p) = NULL; } while(0)
#define SAFE_RELEASE(p)		do { if ((p)) (p)->Release(); (p) = NULL; } while(0)

#define DEBUG_TRACE		DebugTrace
#define DEBUG_VS_TRACE	OutputDebugString

inline void DebugTrace(const char* format, ...)
{
	static char buffer[1024];
	va_list ap;
	va_start(ap, format);
	vsnprintf_s(buffer, sizeof(buffer), format, ap);
	va_end(ap);
	buffer[1023] = 0;
	std::cout << buffer;
}

// string copy
inline void rd_strncpy(char* pDest, const char* pSrc, int destSize)
{
	if (! pDest || ! pSrc)
		return;
	int len = strlen(pSrc) + 1;
	if (len > destSize)
		len = destSize;
	memcpy(pDest, pSrc, len);
	*(pDest + len - 1) = 0;
}

inline const char* avar(const char* format, ...)
{
	static char buffer[1024];
	va_list ap;
	va_start(ap, format);
	vsnprintf_s(buffer, sizeof(buffer), format, ap);
	va_end(ap);
	buffer[1023] = 0;
	return buffer;
};

#define  count_of(n)  (sizeof((n)) / sizeof((n)[0]))

template <class T>	struct Vec2_T
{
	T x, y;

	Vec2_T() : x(0), y(0) {}
	Vec2_T(T _x, T _y) : x(_x), y(_y) {}
	Vec2_T(const Vec2_T<T>& rhs) { x = rhs.x; y = rhs.y; }
	Vec2_T(const Vec2_T<T>* p) { x = p->x; y = p->y; }

	// op
	Vec2_T& operator=(const Vec2_T<T>& rhs) { x = rhs.x; y = rhs.y; return *this; }
	Vec2_T operator-() const { return Vec2_T(-x, -y); }
	Vec2_T operator*(T k) const { return Vec2_T(x * k, y * k); }
	Vec2_T& operator*=(T k) { x *= k; y *= k; return *this }
	Vec2_T operator/(T k) const { return *this * (T)1.0 / k; }
	Vec2_T& operator/=(T k) { return *this *= (T)1.0 / k; }


	void Set(T _x, T _y) { x = _x; y = _y; }
};

template<class T>
Vec2_T<T> operator*(T k, const Vec2_T<T>& rhs)
{
	return Vec2_T(k * rhs.x, k * rhs.y);
}

template<class T1, class T2>
Vec2_T<T1> operator+(const Vec2_T<T1>& lhs,const Vec2_T<T2>& rhs)
{
	return Vec2_T<T1>(lhs.x + rhs.x, lhs.y + rhs.y);
}

template<class T1, class T2>
Vec2_T<T1> operator-(const Vec2_T<T1>& lhs,const Vec2_T<T2>& rhs)
{
	return Vec2_T<T1>(lhs.x - rhs.x, lhs.y - rhs.y);
}

template<class T1, class T2>
Vec2_T<T1>& operator+=(Vec2_T<T1>& lhs,const Vec2_T<T2>& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

template<class T1, class T2>
Vec2_T<T1>& operator-=(Vec2_T<T1>& lhs,const Vec2_T<T2>& rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

template<class T>
bool operator==(const Vec2_T<T>& lhs,const Vec2_T<T>& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

template<class T>
bool operator!=(const Vec2_T<T>& lhs,const Vec2_T<T>& rhs)
{
	return ! (lhs == rhs);
}

template<class T>
inline T square_distance(const Vec2_T<T>& pt1, const Vec2_T<T>& pt2)
{
	return (pt1.x - pt2.x) * (pt1.x - pt2.x) + (pt1.y - pt2.y) * (pt1.y - pt2.y);
}

typedef Vec2_T<int>		Vec2I;
typedef Vec2_T<long>	Vec2L;
typedef Vec2_T<float>	Vec2F;


template <class T> struct Rect_T
{
	T left, top, right, bottom;

	Rect_T() : left(0), top(0), right(0), bottom(0) {}
	Rect_T(T l, T t, T r, T b) : left(l), top(t), right(r), bottom(b) {}
	Rect_T(const Rect_T<T>& rhs) { CopyRect(&rhs); }
	Rect_T(const Rect_T<T>* p) { CopyRect(p); }

	// op
	Rect_T& operator=(const Rect_T<T>& rhs) { CopyRect(&rhs); return *this; }
	Rect_T& operator&=(const Rect_T<T>& rhs) { left = max(left, rhs.left); top = max(top, rhs.top); right = min(right, rhs.right); bottom = min(bottom, rhs.bottom); }
	Rect_T& operator|=(const Rect_T<T>& rhs) { left = min(left, rhs.left); top = min(top, rhs.top); right = max(right, rhs.right); bottom = max(bottom, rhs.bottom); }

	void CopyRect(const Rect_T<T>* pSrc) { memcpy(this, pSrc, sizeof(Rect_T<T>)); }
	void SetRect(T l, T t, T r, T b) { left = l; top = t; right = r; bottom = b; }

	T Width() const { return right - left; }
	T Height() const { return bottom - top; }
	Vec2_T<T> CenterPoint() const { return Vec2_T<T>((left + right) / (T)2.0, (top + bottom) / (T)2.0); }

	bool IsRectEmpty() const { return left >= right || top >= bottom; }
	template<class T1> bool PtInRect (const Vec2_T<T1>& pt) const { return pt.x >= left && pt.x <= right && pt.y >= top && pt.y <= bottom; }

	template<class T1> void OffsetRect(T1 x, T1 y) { left += (T)x; right += (T)x; top += (T)y; bottom += (T)y; }
	template<class T1> void OffsetRect(const Vec2_T<T1>& pt) { OffsetRect(pt.x, pt.y); }
	template<class T1> void InflateRect(T1 l, T1 t, T1 r, T1 b) { left -= (T)l; right += (T)r; top -= (T)t; bottom += (T)t; }
};

template<class T>
bool operator==(const Rect_T<T>& lhs, const Rect_T<T>& rhs)
{
	return lhs.left == rhs.left && lhs.top == rhs.top && lhs.right == rhs.right && lhs.bottom == rhs.bottom;
}

template<class T>
bool operator!=(const Rect_T<T>& lhs, const Rect_T<T>& rhs)
{
	return ! (lhs == rhs);
}

template<class T>
Rect_T<T> operator&(const Rect_T<T>& lhs, const Rect_T<T>& rhs)
{
	Rect_T<T> rect;
	rect.left = max(lhs.left, rhs.left);
	rect.right = min(lhs.right, lhs.right);
	rect.top = max(lhs.top, rhs.top);
	rect.bottom = min(lhs.bottom, rhs.bottom);
	return rect;
}

typedef Rect_T<int>		RectI;
typedef Rect_T<long>	RectL;
typedef Rect_T<float>	RectF;


#endif