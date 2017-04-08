#include "puyopuyogame.h"

#ifdef _DEBUG
	int main()
#else
	int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, char * lpCmdLine, int nCmdShow)
#endif
{
	CPuyoPuyoGame theApp;
	if (theApp.Create())
		theApp.Loop();
	return 0;
}



