#ifndef _PUYO_PUYO_H
#define _PUYO_PUYO_H

#include "common.h"
#include "bitmap.h"

class CPuyoPuyo
{
public:
	struct stPuyo 
	{
		enum PuyoType { PUYO_Null, PUYO_Red, PUYO_Green, PUYO_Blue, PUYO_Yellow, };
		PuyoType	type;
		Vec2I		pos;
		CPuyoPuyo*	parent;
		stPuyo() : type(PUYO_Null),parent(NULL) {}
		
		void SetParent(CPuyoPuyo* p) { parent = p; }

		void SetPos(const Vec2I& _pos) { pos = _pos; }

		// Clear type
		void Clear() { type = PUYO_Null; }
	
		// Drop if is empty
		void Drop();

		// Swap two puyos just type
		void SwapPuyos(stPuyo* p);
		
		// Rotate me by a relative puyo, clockwise
		void RotatebyClockwise(const stPuyo* puyo);

		// Rotate me by a relative puyo, counterclockwise
		void RotateByCounterclockwise(const stPuyo* puyo);

		// Move puyo
		void Move(bool left);

		// Draw puyo
		void Draw(float elapsedTime);
	};

	friend struct stPuyo;

	enum Dir { DIR_Null, DIR_Left, DIR_Top, DIR_Right, DIR_Bottom, };

	CPuyoPuyo(int gridwidth, int gridheight);
	~CPuyoPuyo();

	// Is any puyos dropping
	bool IsPuyoDropping() { return m_puyoA || m_puyoB; }

	// Add two puyos to queue
	void AddPuyosToQueue();

	// Add puyos from queue
	void RemovePuyosFromQueue();
	
	// Get a puyo by pos, return NULL if failed
	stPuyo* GetPuyoByPos(int row, int col);
	
	// Get a puyo by relative, return NULL if failed
	stPuyo* GetPuyoByRelative(const stPuyo* puyo, Dir dir);

	// Get dir by relative
	Dir GetPuyoDirByRelative(const stPuyo* puyo, const stPuyo* relative);

	// Get around puyos
	void GetAroundPuyos(std::vector<stPuyo*>& vec, const stPuyo* puyo);

	// Is the group need remove
	bool IsAroundPuyosCanRemove(std::vector<stPuyo*>& vec);

	// Check for link Puyos
	void CheckForLinkPuyos();

	// Remove link puyos
	void RemoveLinkPuyos();

	// Drop puyos if it can
	void DropPuyos();

	// Run
	void Run();

	// Rotate puyo by 'w' or 's'
	void RotatePuyo(bool clockwise);

	// Move puyo by 'a' or 'd'
	void MovePuyo(bool left);

	// Draw puyos
	void DrawPuyos(float elapsedTime);

	// Get bitmap by puyo type
	CBitmap* GetPuyoBitmap(BYTE type);

	// Get score
	DWORD GetScore() { return m_dwScore; }

	bool IsRun() { return m_bRun; }

	void Restart();

	// Fast drop puyo
	void FastDrop();

	// debug
	void Debug();
private:
	CPuyoPuyo(const CPuyoPuyo&);
	CPuyoPuyo& operator=(const CPuyoPuyo&);
private:
	const int				m_gridWidth;
	const int				m_gridHeight;
	std::vector<stPuyo>		m_vecPuyo;
	std::vector<stPuyo*>	m_vecLinkPuyo;

	stPuyo*					m_puyoA;		// One of add two puyos, may be NULL
	stPuyo*					m_puyoB;		// Another one of add two puyos, may be NULL
	std::queue<stPuyo>		m_puyoQueue;	// Add puyo queue
	std::vector<stPuyo>		m_nextPuyo;		// Next puyo

	CBitmap					m_bitmapRedPuyo;
	CBitmap					m_bitmapGreenPuyo;
	CBitmap					m_bitmapBluePuyo;
	CBitmap					m_bitmapYellowPuyo;

	DWORD					m_dwScore;
	bool					m_bRun;
};




#endif
