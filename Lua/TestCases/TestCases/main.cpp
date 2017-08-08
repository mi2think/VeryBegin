#include "luac.h"

#define LUAC_00 0
#define LUAC_01 0
#define LUAC_02 0
#define LUAC_GC 1

int main()
{
#if LUAC_00
	luac_00();
#elif LUAC_01
	luac_01();
#elif LUAC_02
	luac_02();
#elif LUAC_GC
	luac_gc();
#else
#endif

	return 0;
}