#pragma once
#include <stdio.h>
#include <windows.h>

namespace win
{
	inline void PrintThreadHandle(char* param)
	{
		// Windows explain Pseudo-Handle as current thread
		HANDLE h1 = GetCurrentThread();

		HANDLE h2;
		DuplicateHandle(GetCurrentProcess(),
			h1,
			GetCurrentProcess(),
			&h2,
			0,
			FALSE,
			DUPLICATE_SAME_ACCESS);

		printf("Current thread handle of <%s>: %x %x\n", param, h1, h2);

		CloseHandle(h2);
	}

	inline DWORD WINAPI MyThreadMain(LPVOID param)
	{
		printf("%d: Running: %s \n", GetCurrentThreadId(), (const char*)param);

		return 0;
	}

	inline int CreteThreadEntry()
	{
		DWORD dwThreadId = 0;
		HANDLE hThread = CreateThread(NULL,
			0,
			MyThreadMain,
			"Hello, Thread!",
			0,
			&dwThreadId);

		if (!hThread)
		{
			fprintf(stderr, "Thread create failed: %d\n", GetLastError());
			return -1;
		}

		printf("%d: Create thread %x (ID %d) \n", GetCurrentThreadId(), hThread, dwThreadId);

		WaitForSingleObject(hThread, INFINITE);

		DWORD dwExitCode;
		GetExitCodeThread(hThread, &dwExitCode);
		printf("%d: Thread exited: %d\n", GetCurrentThreadId(), dwExitCode);

		CloseHandle(hThread);

		return 0;
	}
}

