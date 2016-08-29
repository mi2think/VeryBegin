/********************************************************************
	created:	2016/07/06
	created:	6:7:2016   22:25
	filename: 	D:\Code\TinyRay\TinyRay\core\log.cpp
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	log
	file ext:	cpp
	author:		mi2think@gmail.com

	purpose:	
*********************************************************************/
#include "log.h"

#include <windows.h>
#include <iostream>
#include <cstdarg>

namespace tinyray
{
	Log::Log()
		: hwnd_(nullptr)
		, newline_(true)
	{}

	void Log::WriteBuf(int level, const char* fmt, ...)
	{
		SetTextColor(GetTextColor(level));

		static char buffer[1024];
		va_list ap;
		va_start(ap, fmt);
		vsprintf_s(buffer, fmt, ap);
		va_end(ap);
		buffer[1023] = 0;

		std::ostream* os = nullptr;
		if (level == Error || level == Fatal)
			os = &std::cout;
		else
			os = &std::cerr;
		(*os) << buffer;
		if (newline_)
			(*os) << std::endl;
	}

	unsigned short Log::GetTextColor(int level)
	{
		unsigned short color;
		switch (level)
		{
		case Info:
			// info is bright white
			color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			break;
		case Warn:
			// warn is bright yellow
			color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			break;
		case Error:
			// error is red
			color = FOREGROUND_RED | FOREGROUND_INTENSITY;
			break;
		case Fatal:
			// fatal is white on red
			color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_INTENSITY;
			break;
		default:
			// other is cyan
			color = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			break;
		}
		return color;
	}

	void Log::SetTextColor(unsigned short color)
	{
		if (hwnd_ == NULL)
		{
			hwnd_ = GetStdHandle(STD_OUTPUT_HANDLE);
		}
		SetConsoleTextAttribute(hwnd_, color);
	}

	Log::ScopeNewline::ScopeNewline(bool newline)
	{
		newline_ = GLOG.GetNewline();
		GLOG.SetNewline(newline);
	}

	Log::ScopeNewline::~ScopeNewline()
	{
		GLOG.SetNewline(newline_);
	}
}