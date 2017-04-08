#ifndef _PUYO_PUYO_GAME_H
#define _PUYO_PUYO_GAME_H

#include "puyopuyo.h"

class CPuyoPuyoGame
{
public:
	CPuyoPuyoGame();
	~CPuyoPuyoGame();

	bool Create();

	void Run(float elapsedTime);

	void Draw(float elapsedTime);

	bool MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void Loop();
private:
	CPuyoPuyo	m_Puyopuyo;
	float		m_fMoveTime;
};

#endif
