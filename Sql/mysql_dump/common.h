#pragma once
#include <functional>
#include <algorithm>

#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <set>

#include <cassert>
#include <cstdarg>
#include <ctime>

class ScopeGuard
{
public:
	ScopeGuard(std::function<void()> func) : func_(func) {}
	~ScopeGuard() { func_(); }

private:
	ScopeGuard(const ScopeGuard&);
	ScopeGuard& operator=(const ScopeGuard&);

	std::function<void()> func_;
};

#define SCOPEGUARD_NAMELINE_CAT(name, line) name##line
#define SCOPEGUARD_NAMELINE(name, line) SCOPEGUARD_NAMELINE_CAT(name, line)
#define ON_SCOPE_EXIT(func) ScopeGuard SCOPEGUARD_NAMELINE(EXIT, __LINE__)(func)


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

inline void safe_strncpy(char* pDest, const char* pSrc, int destSize)
{
	if (! pDest || ! pSrc)
		return;
	int len = strlen(pSrc) + 1;
	if (len > destSize)
		len = destSize;
	memcpy(pDest, pSrc, len);
	*(pDest + len - 1) = 0;
}