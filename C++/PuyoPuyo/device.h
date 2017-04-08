#ifndef _DEVICE_H
#define _DEVICE_H

#include "common.h"
#include <d3d9.h>
#include <d3dx9core.h>

struct stVertex
{
	float x, y, z, w;
	DWORD color;
	static const DWORD FVF;
};

struct stVertexTex
{
	float x, y, z, w;
	DWORD color;
	float u, v;
	static const DWORD FVF;
};

struct stQuadTex
{
	stVertexTex vt[4];
	IDirect3DTexture9* tex;
};

class CDevice
{
public:
	CDevice();
	~CDevice();

	// Init device, return true if success, else false
	bool Init(HWND hwnd);

	// Release device
	void Release();

	// Begin scene
	void BeginScene(DWORD color);

	// End scene
	void EndScene();

	// Draw string
	void DrawString(const char* text, const RectI& rect, DWORD format, DWORD color);

	// Draw a line
	void DrawLine(const Vec2F& pt0, const Vec2F& pt1, DWORD color);
	
	// Draw line list
	void DrawLineList(const Vec2F* p, DWORD num, DWORD color);

	// Draw a bitmap, with a quad
	void DrawQuadTex(const stQuadTex& qt);

	// Flush Quad buffer
	void FlushQuadTex();

	// Load Tex by file name
	void LoadTexByFileName(const std::string& fileName);

	// Get Tex By file name
	IDirect3DTexture9* GetTexByFileName(const std::string& fileName);

	// Debug
	void Debug();
private:
	D3DADAPTER_IDENTIFIER9	m_adapterIdentifer;
	D3DDISPLAYMODE			m_displayMode;
	D3DCAPS9				m_caps;

	HWND					m_hWnd;
	IDirect3D9*				m_pD3D;
	IDirect3DDevice9*		m_pD3DD;
	IDirect3DVertexBuffer9*	m_pVB;
	IDirect3DIndexBuffer9*	m_pIB;
	
	ID3DXFont*				m_pTextOut;

	DWORD					m_dwVBOffset;
	DWORD					m_dwIBOffset;

	std::map<void*,std::vector<stVertexTex> >	m_mapQuadTex;
	std::map<std::string, IDirect3DTexture9*>	m_mapFileTex;
};

inline CDevice* GetDevice() { static CDevice device; return &device; }

#endif