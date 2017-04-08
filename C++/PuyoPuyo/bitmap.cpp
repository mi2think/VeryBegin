#include "bitmap.h"
#include "device.h"

CBitmap::CBitmap(const char* fileName, int width, int height)
: m_fileName(fileName)
, m_width(width)
, m_height(height)
{

}

CBitmap::~CBitmap()
{

}

void CBitmap::Draw(const Vec2F& pos)
{
	if (m_tex == NULL)
	{
		GetDevice()->LoadTexByFileName(m_fileName);
		m_tex = GetDevice()->GetTexByFileName(m_fileName);
	}

	stQuadTex qt;
	qt.vt[0].x = pos.x;				qt.vt[0].y = pos.y;				qt.vt[0].z = 0.0f;		qt.vt[0].w = 1.0f;	qt.vt[0].u = 0.0f;	qt.vt[0].v = 0.0f; qt.vt[0].color = 0xffffffff;
	qt.vt[1].x = pos.x + m_width;	qt.vt[1].y = pos.y;				qt.vt[1].z = 0.0f;		qt.vt[1].w = 1.0f;	qt.vt[1].u = 1.0f;	qt.vt[1].v = 0.0f; qt.vt[1].color = 0xffffffff;
	qt.vt[2].x = pos.x + m_width;	qt.vt[2].y = pos.y + m_height;	qt.vt[2].z = 0.0f;		qt.vt[2].w = 1.0f;	qt.vt[2].u = 1.0f;	qt.vt[2].v = 1.0f; qt.vt[2].color = 0xffffffff;
	qt.vt[3].x = pos.x;				qt.vt[3].y = pos.y + m_height;	qt.vt[3].z = 0.0f;		qt.vt[3].w = 1.0f;	qt.vt[3].u = 0.0f;	qt.vt[3].v = 1.0f; qt.vt[3].color = 0xffffffff;
	qt.tex = m_tex;

	GetDevice()->DrawQuadTex(qt);
}