#include "puyopuyo.h"
#include "device.h"
#include <stack>
#include <cassert>
#include <functional>

#define PNG_WIDTH	32
#define PNG_HEIGHT	32

//////////////////////////////////////////////////////////////////////////

void CPuyoPuyo::stPuyo::Drop()
{
	if (type == PUYO_Null)
		return;
	stPuyo* puyo = parent->GetPuyoByRelative(this, DIR_Bottom);

	if (! puyo || puyo->type != PUYO_Null)
	{
		if (this == parent->m_puyoA)
			 parent->m_puyoA = NULL;
		else if (this == parent->m_puyoB)
			parent->m_puyoB = NULL;
	}
	else
		SwapPuyos(puyo);
};

void CPuyoPuyo::stPuyo::SwapPuyos(stPuyo* p)
{
	if (! p || p->type != PUYO_Null)
		return;

	PuyoType t = type;
	type = p->type;
	p->type = t;

	if (this == parent->m_puyoA)
		parent->m_puyoA = p;
	else if (this == parent->m_puyoB)
		parent->m_puyoB = p;
}

void CPuyoPuyo::stPuyo::RotatebyClockwise(const stPuyo* puyo)
{
	Dir dir = parent->GetPuyoDirByRelative(this, puyo);

	stPuyo* dest = NULL;
	switch(dir)
	{
	case DIR_Left:
		dest = parent->GetPuyoByRelative(puyo, DIR_Top);
		break;
	case DIR_Top:
		dest = parent->GetPuyoByRelative(puyo, DIR_Right);
		break;
	case DIR_Right:
		dest = parent->GetPuyoByRelative(puyo, DIR_Bottom);
		break;
	case DIR_Bottom:
		dest = parent->GetPuyoByRelative(puyo, DIR_Left);
		break;
	}

	SwapPuyos(dest);
}

void CPuyoPuyo::stPuyo::RotateByCounterclockwise(const stPuyo* puyo)
{
	Dir dir = parent->GetPuyoDirByRelative(this, puyo);
	
	stPuyo* dest = NULL;
	switch(dir)
	{
	case DIR_Left:
		dest = parent->GetPuyoByRelative(puyo, DIR_Bottom);
		break;
	case DIR_Top:
		dest = parent->GetPuyoByRelative(puyo, DIR_Left);
		break;
	case DIR_Right:
		dest = parent->GetPuyoByRelative(puyo, DIR_Top);
		break;
	case DIR_Bottom:
		dest = parent->GetPuyoByRelative(puyo, DIR_Right);
		break;
	}

	SwapPuyos(dest);
}

void CPuyoPuyo::stPuyo::Move(bool left)
{
	stPuyo* puyo = parent->GetPuyoByRelative(this, left ? DIR_Left : DIR_Right);
	SwapPuyos(puyo);
}

void CPuyoPuyo::stPuyo::Draw(float elapsedTime)
{
	if (type == PUYO_Null)
		return;
	CBitmap* bitmap = parent->GetPuyoBitmap(type);
	if (bitmap)
		bitmap->Draw(Vec2F(pos.x * PNG_WIDTH, pos.y * PNG_HEIGHT));
}

//////////////////////////////////////////////////////////////////////////

CPuyoPuyo::CPuyoPuyo(int gridwidth, int gridheight)
: m_puyoA(NULL)
, m_puyoB(NULL)
, m_gridWidth(gridwidth)
, m_gridHeight(gridheight)
, m_bitmapRedPuyo("data\\puyo_red.png", PNG_WIDTH, PNG_HEIGHT)
, m_bitmapGreenPuyo("data\\puyo_green.png", PNG_WIDTH, PNG_HEIGHT)
, m_bitmapBluePuyo("data\\puyo_blue.png", PNG_WIDTH, PNG_HEIGHT)
, m_bitmapYellowPuyo("data\\puyo_yellow.png", PNG_WIDTH, PNG_HEIGHT)
, m_dwScore(0)
, m_bRun(true)
{
	m_vecPuyo.resize(gridwidth * gridheight);
	for (int i = 0; i < m_gridHeight; ++i)
	{
		for (int j = 0; j < m_gridWidth; ++j)
		{
			stPuyo& puyo = m_vecPuyo[i * m_gridWidth + j];
			puyo.SetParent(this);
			puyo.SetPos(Vec2I(j, i));
		}
	}
}

CPuyoPuyo::~CPuyoPuyo()
{

}

void CPuyoPuyo::AddPuyosToQueue()
{
	if (m_nextPuyo.empty())
	{
		stPuyo puyoB;
		puyoB.type =  (stPuyo::PuyoType)(rand() % 4 + 1);

		stPuyo puyoA;
		puyoA.type =  (stPuyo::PuyoType)(rand() % 4 + 1);

		m_nextPuyo.push_back(puyoA);
		m_nextPuyo.push_back(puyoB);
	}
	else
	{
		m_puyoQueue.push(m_nextPuyo[1]);
		m_puyoQueue.push(m_nextPuyo[0]);

		m_nextPuyo.clear();

		stPuyo puyoB;
		puyoB.type = (stPuyo::PuyoType)(rand() % 4 + 1);

		stPuyo puyoA;
		puyoA.type = (stPuyo::PuyoType)(rand() % 4 + 1);

		m_nextPuyo.push_back(puyoA);
		m_nextPuyo.push_back(puyoB);
	}
}

void CPuyoPuyo::RemovePuyosFromQueue()
{
	if (m_puyoQueue.empty())
		return;

	int qSize = m_puyoQueue.size();
	stPuyo* puyoA = GetPuyoByPos(0, 2);
	stPuyo* puyoB = GetPuyoByPos(1, 2);

	if (qSize == 1)
	{
		if (puyoA->type == stPuyo::PUYO_Null)
		{
			stPuyo puyo1 = m_puyoQueue.front();
			m_puyoQueue.pop();
			puyoA->type = puyo1.type;
			m_puyoA = puyoA;
		}
		else
		{
			m_bRun = false;
		}
	}
	else if (qSize == 2)
	{
		if (puyoA->type == stPuyo::PUYO_Null && puyoB->type == stPuyo::PUYO_Null)
		{
			stPuyo puyo1 = m_puyoQueue.front();
			m_puyoQueue.pop();
			puyoB->type = puyo1.type;
			m_puyoB = puyoB;

			stPuyo puyo2 = m_puyoQueue.front();
			m_puyoQueue.pop();
			puyoA->type = puyo2.type;
			m_puyoA = puyoA;
		}
		else if (puyoA->type == stPuyo::PUYO_Null)
		{
			stPuyo puyo1 = m_puyoQueue.front();
			m_puyoQueue.pop();
			puyoB->type = puyo1.type;
			m_puyoB = puyoB;
		}
		else
		{
			m_bRun = false;
		}
	}
}

CPuyoPuyo::stPuyo* CPuyoPuyo::GetPuyoByPos(int row, int col)
{
	if (row >= 0 && row < m_gridHeight && col >= 0 && col < m_gridWidth)
		return &m_vecPuyo[row * m_gridWidth + col];

	return NULL;
}

CPuyoPuyo::stPuyo* CPuyoPuyo::GetPuyoByRelative(const stPuyo* puyo, Dir dir)
{
	assert(puyo != NULL);
	
	Vec2I pos = puyo->pos;
	switch (dir)
	{
	case DIR_Left:
		pos.x -= 1;
		break;
	case DIR_Top:
		pos.y -= 1;
		break;
	case DIR_Right:
		pos.x += 1;
		break;
	case DIR_Bottom:
		pos.y += 1;
		break;
	}

	return GetPuyoByPos(pos.y, pos.x);
}

CPuyoPuyo::Dir CPuyoPuyo::GetPuyoDirByRelative(const stPuyo* puyo, const stPuyo* relative)
{
	assert(puyo != NULL && relative != NULL && puyo != relative);

	if (puyo->pos.x == relative->pos.x)
	{
		if (puyo->pos.y < relative->pos.y)
			return DIR_Top;
		else
			return DIR_Bottom;
	}
	else if (puyo->pos.y == relative->pos.y)
	{
		if (puyo->pos.x < relative->pos.x)
			return DIR_Left;
		else
			return DIR_Right;
	}

	return DIR_Null;
}

void CPuyoPuyo::GetAroundPuyos(std::vector<stPuyo*>& vec, const stPuyo* puyo)
{
	stPuyo* pL = GetPuyoByRelative(puyo, DIR_Left);
	if (pL && pL->type == puyo->type)
		vec.push_back(pL);
	stPuyo* pT = GetPuyoByRelative(puyo, DIR_Top);
	if (pT && pT->type == puyo->type)
		vec.push_back(pT);
	stPuyo* pR = GetPuyoByRelative(puyo, DIR_Right);
	if (pR && pR->type == puyo->type)
		vec.push_back(pR);
	stPuyo* pB = GetPuyoByRelative(puyo, DIR_Bottom);
	if (pB && pB->type == puyo->type)
		vec.push_back(pB);
}

bool CPuyoPuyo::IsAroundPuyosCanRemove(std::vector<stPuyo*>& vec)
{
	// If any one of around puyos can drop. return false
	for(auto e: vec)
	{
		Vec2I pos = e->pos;
		for (int i = pos.y; i < m_gridHeight; ++i)
		{
			const stPuyo* down = GetPuyoByPos(i, pos.x);
			if (down && down->type == stPuyo::PUYO_Null)
				return false;
		}
	};

	return true;
}

void CPuyoPuyo::CheckForLinkPuyos()
{
	std::set<stPuyo*> setWalkPuyo;

	std::for_each(m_vecPuyo.begin(), m_vecPuyo.end(), [&](decltype(*m_vecPuyo.begin())& puyo)
	{
		if (puyo.type != stPuyo::PUYO_Null)
		{
			// Push first puyo
			std::stack<stPuyo*> s;
			s.push(&puyo);

			// Go, find link puyos
			std::vector<stPuyo*> vec_link;
			while (! s.empty())
			{
				stPuyo* p = s.top();
				s.pop();

				std::vector<stPuyo*> vec_around;
				this->GetAroundPuyos(vec_around, p);
				for (auto e : vec_around)
				{
					auto it = setWalkPuyo.find(e);
					if (it == setWalkPuyo.end())
					{
						setWalkPuyo.insert(e);

						if (e->type == p->type)
						{
							s.push(e);
							vec_link.push_back(e);
						}
					}
				}
			}

			// >=4, can remove it
			if (vec_link.size() >= 4 && IsAroundPuyosCanRemove(vec_link))
				m_vecLinkPuyo.insert(m_vecLinkPuyo.end(), vec_link.begin(), vec_link.end());
		}
	});
}

void CPuyoPuyo::RemoveLinkPuyos()
{
	if (! m_vecLinkPuyo.empty())
	{
		std::for_each(m_vecLinkPuyo.begin(), m_vecLinkPuyo.end(), [this](decltype(*m_vecLinkPuyo.begin()) p)
		{ 
			p->type = stPuyo::PUYO_Null;
			if (p == m_puyoA)
				m_puyoA = NULL;
			else if (p == m_puyoB)
				m_puyoB = NULL;
		});
		m_dwScore += m_vecLinkPuyo.size() * 10;
		m_vecLinkPuyo.clear();
	}
}

void CPuyoPuyo::DropPuyos()
{
	for (int i = m_vecPuyo.size() - 1; i >= 0; --i)
	{
		m_vecPuyo[i].Drop();
	}
}

void CPuyoPuyo::Run()
{
	if (! m_bRun)
		return;

	CheckForLinkPuyos();

	RemoveLinkPuyos();

	if (! IsPuyoDropping() && m_puyoQueue.empty())
	{
		AddPuyosToQueue();
	}

	if (! m_puyoQueue.empty())
	{
		RemovePuyosFromQueue();
	}
	else
	{
		DropPuyos();
	}

	Debug();
}

void CPuyoPuyo::RotatePuyo(bool clockwise)
{
	if (m_puyoA && m_puyoB)
	{
		if (clockwise)
			m_puyoA->RotatebyClockwise(m_puyoB);
		else
			m_puyoA->RotateByCounterclockwise(m_puyoB);
	}
}

void CPuyoPuyo::MovePuyo(bool left)
{
	if (m_puyoA && m_puyoB)
	{
		if (left)
		{
			if (m_puyoA->pos.x <=  m_puyoB->pos.x)
			{
				m_puyoA->Move(left);
				m_puyoB->Move(left);
			}
			else
			{
				m_puyoB->Move(left);
				m_puyoA->Move(left);
			}
		}
		else
		{
			if (m_puyoA->pos.x >=  m_puyoB->pos.x)
			{
				m_puyoA->Move(left);
				m_puyoB->Move(left);
			}
			else
			{
				m_puyoB->Move(left);
				m_puyoA->Move(left);
			}
		}
	}
	else
	{
		if (m_puyoA)
			m_puyoA->Move(left);
		if (m_puyoB)
			m_puyoB->Move(left);
	}
}

void CPuyoPuyo::DrawPuyos(float elapsedTime)
{
	std::for_each(m_vecPuyo.begin(), m_vecPuyo.end(), [=](decltype(*m_vecPuyo.begin())& puyo){ puyo.Draw(elapsedTime); });

	// next puyos
	if (! m_nextPuyo.empty())
	{
		stPuyo& puyo1 = m_nextPuyo[0];
		puyo1.SetPos(Vec2I(m_gridWidth + 1, 8));
		puyo1.SetParent(this);
		puyo1.Draw(elapsedTime);

		stPuyo& puyo2 = m_nextPuyo[1];
		puyo2.SetPos(Vec2I(m_gridWidth + 1, 9));
		puyo2.SetParent(this);
		puyo2.Draw(elapsedTime);
	}
}

CBitmap* CPuyoPuyo::GetPuyoBitmap(BYTE type)
{
	CBitmap* bitmap = NULL;
	switch (type)
	{
	case stPuyo::PUYO_Red:
		bitmap = &m_bitmapRedPuyo;
		break;
	case stPuyo::PUYO_Green:
		bitmap = &m_bitmapGreenPuyo;
		break;
	case stPuyo::PUYO_Blue:
		bitmap = &m_bitmapBluePuyo;
		break;
	case stPuyo::PUYO_Yellow:
		bitmap = &m_bitmapYellowPuyo;
		break;
	}

	return bitmap;
}

void CPuyoPuyo::Restart()
{
	m_bRun = true;
	m_dwScore = 0;
	m_puyoA = NULL;
	m_puyoB = NULL;
	std::for_each(m_vecPuyo.begin(), m_vecPuyo.end(), std::mem_fun_ref(&stPuyo::Clear));
	m_vecLinkPuyo.clear();
	while (! m_puyoQueue.empty())
	{
		m_puyoQueue.pop();
	}
	m_nextPuyo.clear();
}

void CPuyoPuyo::FastDrop()
{
	if (! m_puyoA && ! m_puyoB)
		return;

	auto fnSwap = [this](stPuyo* puyo)
	{
		stPuyo* val = NULL;
		Vec2I pos = puyo->pos;
		for (int i = pos.y; i < m_gridHeight; ++i)
		{
			stPuyo* p = GetPuyoByPos(i, pos.x);
			if (p && p->type == stPuyo::PUYO_Null)
			{
				val = p;
			}
		}

		if (val != NULL)
		{
			puyo->SwapPuyos(val);
		}
	};

	if (m_puyoA && m_puyoB)
	{
		Dir dir = GetPuyoDirByRelative(m_puyoA, m_puyoB);
		switch (dir)
		{
		case DIR_Left:
		case DIR_Right:
		case DIR_Bottom:
			fnSwap(m_puyoA);
			fnSwap(m_puyoB);
			break;
		case DIR_Top:
			fnSwap(m_puyoB);
			fnSwap(m_puyoA);
			break;
		}
	}
	else if (m_puyoA)
	{
		fnSwap(m_puyoA);
	}
	else if (m_puyoB)
	{
		fnSwap(m_puyoB);
	}
}

void CPuyoPuyo::Debug()
{
	std::cout << "----------------------\n";
	for (int i = 0; i < m_gridHeight; ++i)
	{
		for (int j = 0; j < m_gridWidth; ++j)
		{
			stPuyo& puyo = m_vecPuyo[i * m_gridWidth + j];
			std::cout << puyo.type << " ";
		}
		std::cout << "\n";
	}
}
