#include "thread_create_win.h"

#define USE_WIN 1

int main()
{
#if USE_WIN
	return win::CreteThreadEntry();
#elif
#endif
}
