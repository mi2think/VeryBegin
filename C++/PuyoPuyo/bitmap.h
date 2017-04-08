#ifndef _BITMAP_H
#define _BITMAP_H

#include "common.h"
#include <d3d9.h>

class CBitmap
{
public:
	CBitmap(const char* fileName, int width, int height);
	~CBitmap();

	void Draw(const Vec2F& pos);
private:
	std::string			m_fileName;
	IDirect3DTexture9*	m_tex;
	int					m_width;
	int					m_height;
};


#endif