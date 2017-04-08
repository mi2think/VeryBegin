#include "puyopuyogame.h"
#include "device.h"
#include <chrono>

#define GRID_WIDTH		6
#define GRID_HEIGTH		12
#define ICON_WIDTH		32

#define WINDOW_WIDTH	(ICON_WIDTH * GRID_WIDTH + 180)
#define WINDOW_HEIGHT	(ICON_WIDTH * GRID_HEIGTH)

HWND g_hWnd = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPuyoPuyoGame* app = (CPuyoPuyoGame*)GetWindowLong(g_hWnd, GWL_USERDATA);
	if (app && g_hWnd == hWnd)
		app->MsgProc(hWnd, uMsg, wParam, lParam);
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

CPuyoPuyoGame::CPuyoPuyoGame()
: m_fMoveTime(0.0f)
, m_Puyopuyo(GRID_WIDTH, GRID_HEIGTH)
{

}

CPuyoPuyoGame::~CPuyoPuyoGame()
{

}

bool CPuyoPuyoGame::Create()
{
	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = NULL;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = "puyopuyo_class";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	
	if (! RegisterClass(&wc))
	{
		std::cerr << "RegisterClass failed!\n";
		return false;
	}

	g_hWnd = CreateWindow(wc.lpszClassName, "puyopuyo", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
		ICON_WIDTH * GRID_WIDTH, ICON_WIDTH * GRID_HEIGTH, NULL, NULL, wc.hInstance, NULL);
	if (! g_hWnd)
	{
		std::cerr << "CreateWindow failed!\n";
		return false;
	}

	RECT windowRect;
	GetWindowRect(g_hWnd, &windowRect); 
	RECT ClientRect;
	GetClientRect(g_hWnd, &ClientRect);  
	windowRect.right += (WINDOW_WIDTH - ClientRect.right);  
	windowRect.bottom += (WINDOW_HEIGHT - ClientRect.bottom);

	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);
	int widthWindow = windowRect.right - windowRect.left;
	int heightWindow = windowRect.bottom - windowRect.top;
	MoveWindow(g_hWnd, (cxScreen - widthWindow) / 2, (cyScreen - heightWindow) / 2, widthWindow, heightWindow, true);  

	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);
	SetWindowLong(g_hWnd, GWL_USERDATA, (LONG)this);

	GetDevice()->Init(g_hWnd);

	srand((unsigned int)time(NULL));

	return true;
}

void CPuyoPuyoGame::Run(float elapsedTime)
{
	m_fMoveTime += elapsedTime;

	if (m_fMoveTime > 0.3f)
	{
		m_Puyopuyo.Run();

		m_fMoveTime = 0.0f;
	}
}

bool CPuyoPuyoGame::MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CHAR:
		switch (wParam)
		{
		case 'W':
		case 'S':
		case 'w':
		case 's':
			m_Puyopuyo.RotatePuyo(tolower(wParam) == 's');
			break;
		case 'A':
		case 'D':
		case 'a':
		case 'd':
			m_Puyopuyo.MovePuyo(tolower(wParam) == 'a');
			break;
		case 'R':
		case 'r':
			m_Puyopuyo.Restart();
			break;
		case 'f':
		case 'F':
			m_Puyopuyo.FastDrop();
			break;
		}
		break;
	}
	return true;
}

void CPuyoPuyoGame::Loop()
{
	auto lastTime = std::chrono::high_resolution_clock::now();
	while (true)
	{
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			auto currTime  = std::chrono::high_resolution_clock::now();
			auto elapsedUS = std::chrono::duration_cast<std::chrono::milliseconds>(currTime - lastTime).count();
			auto elapsedTime = elapsedUS / 1000.0f;
			Run(elapsedTime);
			GetDevice()->BeginScene(0xffffffff);
			Draw(elapsedTime);
			GetDevice()->EndScene();

			lastTime = currTime;
		}
	}

	GetDevice()->Release();
}

void CPuyoPuyoGame::Draw(float elapsedTime)
{
	m_Puyopuyo.DrawPuyos(elapsedTime);

	Vec2F pt0(ICON_WIDTH * GRID_WIDTH, 0);
	Vec2F pt1(ICON_WIDTH * GRID_WIDTH, WINDOW_HEIGHT);
	GetDevice()->DrawLine(pt0, pt1, 0x00000000);


	// text
	RectI rcWelcome(ICON_WIDTH * GRID_WIDTH + 5, 10, WINDOW_WIDTH, 30);
	GetDevice()->DrawString("Welcome to puyo_puyo!", rcWelcome, DT_LEFT | DT_TOP, 0xffff0000);
	RectI rcTip(ICON_WIDTH * GRID_WIDTH + 5, 50, WINDOW_WIDTH, 70);
	GetDevice()->DrawString("Press key:", rcTip, DT_LEFT | DT_TOP, 0xff000000);
	rcTip.OffsetRect(0, 20);
	GetDevice()->DrawString("A: move to left", rcTip, DT_LEFT | DT_TOP, 0xff000000);
	rcTip.OffsetRect(0, 20);
	GetDevice()->DrawString("D: move to right", rcTip, DT_LEFT | DT_TOP, 0xff000000);
	rcTip.OffsetRect(0, 20);
	GetDevice()->DrawString("W: rotate clockwise", rcTip, DT_LEFT | DT_TOP, 0xff000000);	
	rcTip.OffsetRect(0, 20);
	GetDevice()->DrawString("S: rotate counterclockwise", rcTip, DT_LEFT | DT_TOP, 0xff000000);
	rcTip.OffsetRect(0, 20);
	GetDevice()->DrawString("F: Fast drop", rcTip, DT_LEFT | DT_TOP, 0xff000000);
	rcTip.OffsetRect(0, 20);
	GetDevice()->DrawString("R: Restart game", rcTip, DT_LEFT | DT_TOP, 0xff000000);

	rcTip.OffsetRect(0, 30);
	GetDevice()->DrawString(avar("Your scores:%d", m_Puyopuyo.GetScore()), rcTip, DT_LEFT | DT_TOP, 0xff000000);
	rcTip.OffsetRect(0, 20);
	if (! m_Puyopuyo.IsRun())
		GetDevice()->DrawString("You failed!", rcTip, DT_LEFT | DT_TOP, 0xffff0000);

	rcTip.OffsetRect(0, 20);
	GetDevice()->DrawString("Next puyos:", rcTip, DT_LEFT | DT_TOP, 0xffff0000);
}

