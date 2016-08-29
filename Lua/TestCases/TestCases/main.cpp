#include "luac.h"

#define LUAC_00 0
#define LUAC_01 0
#define LUAC_02 1

int main()
{
#if LUAC_00
	luac_00();
#elif LUAC_01
	luac_01();
#elif LUAC_02
	luac_02();
#else
#endif

	return 0;
}