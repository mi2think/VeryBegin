#include "device.h"
#include <DxErr.h>

#define DEBUG_DXTRACE(hr)	DebugDXTrace(hr, __FILE__, __LINE__)

// hack for dxerr.lib for it not compatible with vs2015
int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;

void DebugDXTrace(HRESULT hr, const char* file, int line)
{
	const char* error = DXGetErrorDescription(hr);
	DEBUG_TRACE("Dx Error: %s:%d %s\n", file, line, error);
}

const DWORD stVertex::FVF		= D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
const DWORD stVertexTex::FVF	= D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE;
const DWORD vertexBufferSize = 5000 * sizeof(stVertexTex) * 4;
const DWORD indexBufferSize  = 5000 * sizeof(WORD) * 6;


CDevice::CDevice()
: m_pD3D(NULL)
, m_pD3DD(NULL)
, m_pVB(NULL)
, m_pIB(NULL)
, m_pTextOut(NULL)
, m_dwVBOffset(0)
, m_dwIBOffset(0)
{

}

CDevice::~CDevice()
{

}

bool CDevice::Init(HWND hwnd)
{
	m_hWnd = hwnd;

	// Query d3d interface
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (! m_pD3D)
	{
		DEBUG_TRACE("Error: Direct3DCreate9\n");
		return false;
	}

	DWORD adapterCount = m_pD3D->GetAdapterCount();
	DEBUG_TRACE("adapter count:%d\n", adapterCount);

	// Check adapter info
	HRESULT hr;
	hr = m_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &m_adapterIdentifer);
	if (FAILED(hr))
	{
		DEBUG_DXTRACE(hr);
		return false;
	}
	DEBUG_TRACE("Adapter info:\nDirver:%s\nDescription:%s", m_adapterIdentifer.Driver, m_adapterIdentifer.Description);
	DWORD Product = HIWORD(m_adapterIdentifer.DriverVersion.HighPart);
	DWORD Version = LOWORD(m_adapterIdentifer.DriverVersion.HighPart);
	DWORD SubVersion = HIWORD(m_adapterIdentifer.DriverVersion.LowPart);
	DWORD Build = LOWORD(m_adapterIdentifer.DriverVersion.LowPart);
	DEBUG_TRACE("\t%d.%d.%d.%d\n", Product, Version, SubVersion, Build);

	// Check adapter mode count
	hr = m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_displayMode);
	if (FAILED(hr))
	{
		DEBUG_DXTRACE(hr);
		return false;
	}

	auto fnDisplay = [](D3DDISPLAYMODE& displayMode)
	{
		std::string fmt;
		switch (displayMode.Format)
		{
		case D3DFMT_R8G8B8:			fmt = "D3DFMT_R8G8B8";			break;
		case D3DFMT_A8R8G8B8:		fmt = "D3DFMT_A8R8G8B8";		break;
		case D3DFMT_X8R8G8B8:		fmt = "D3DFMT_X8R8G8B8";		break;
		case D3DFMT_R5G6B5:			fmt = "D3DFMT_R5G6B5";			break;
		case D3DFMT_X1R5G5B5:		fmt = "D3DFMT_X1R5G5B5";		break;
		case D3DFMT_A1R5G5B5:		fmt = "D3DFMT_A1R5G5B5";		break;
		case D3DFMT_A4R4G4B4:		fmt = "D3DFMT_A4R4G4B4";		break;
		case D3DFMT_R3G3B2:			fmt = "D3DFMT_R3G3B2";			break;
		case D3DFMT_A8:				fmt = "D3DFMT_A8";				break;
		case D3DFMT_A8R3G3B2:		fmt = "D3DFMT_A8R3G3B2";		break;
		case D3DFMT_X4R4G4B4:		fmt = "D3DFMT_X4R4G4B4";		break;
		case D3DFMT_A2B10G10R10:	fmt = "D3DFMT_A2B10G10R10";		break;
		case D3DFMT_A8B8G8R8:		fmt = "D3DFMT_A8B8G8R8";		break;
		case D3DFMT_X8B8G8R8:		fmt = "D3DFMT_X8B8G8R8";		break;
		case D3DFMT_G16R16:			fmt = "D3DFMT_G16R16";			break;
		case D3DFMT_A2R10G10B10:	fmt = "D3DFMT_A2R10G10B10";		break;
		case D3DFMT_A16B16G16R16:	fmt = "D3DFMT_A16B16G16R16";	break;
		}
		DEBUG_TRACE("Width:%d\tHeight:%d\tRefreshRate:%d\tFormat:%s\n", displayMode.Width, displayMode.Height, displayMode.RefreshRate, fmt.c_str());
	};

	fnDisplay(m_displayMode);

	// Check other adapter mode
	DEBUG_TRACE("Other display mode:\n");
	int adapterModeCount = m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
	for (int i = 0; i < adapterModeCount; ++i)
	{
		D3DDISPLAYMODE displayMode;
		if (SUCCEEDED(m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &displayMode)))
		{
			fnDisplay(displayMode);
		}
	}

	// Check device caps
	hr = m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_caps);
	if (FAILED(hr))
	{
		DEBUG_DXTRACE(hr);
		return false;
	}
	DEBUG_TRACE("Dev caps:\n");
	if (m_caps.DevCaps2 & D3DDEVCAPS2_STREAMOFFSET)
		DEBUG_TRACE("Device supports stream offsets \n");
	int vp = 0;
	if (m_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		DEBUG_TRACE("Device supports T & L \n");
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// Create device
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = m_hWnd;
	d3dpp.BackBufferFormat = m_displayMode.Format;
	hr = m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.BackBufferFormat, d3dpp.BackBufferFormat, d3dpp.Windowed);
	if (FAILED(hr))
	{
		DEBUG_DXTRACE(hr);
		return false;
	}
	hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, vp, &d3dpp, &m_pD3DD);
	if (FAILED(hr))
	{
		DEBUG_DXTRACE(hr);
		return false;
	}
	DEBUG_TRACE("Create device successed!\n");

	// Create font
	D3DXFONT_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Height = 12;
	desc.Width = 0;
	desc.Weight = 400;
	desc.MipLevels = D3DX_DEFAULT;
	desc.Italic = false;
	desc.CharSet = DEFAULT_CHARSET;
	desc.OutputPrecision = 0;
	desc.Quality = 0;
	desc.PitchAndFamily = 0;
	rd_strncpy(desc.FaceName, "ËÎÌו", sizeof(desc.FaceName));

	hr = D3DXCreateFontIndirect(m_pD3DD, &desc, &m_pTextOut);
	if (FAILED(hr))
	{
		DEBUG_DXTRACE(hr);
		return false;
	}
	DEBUG_TRACE("Create font successed!\n");

	// Create vertex & index buffer
	hr = m_pD3DD->CreateVertexBuffer(vertexBufferSize, D3DUSAGE_WRITEONLY, stVertexTex::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL);
	if (FAILED(hr))
	{
		DEBUG_DXTRACE(hr);
		return false;
	}
	hr = m_pD3DD->CreateIndexBuffer(indexBufferSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL);
	if (FAILED(hr))
	{
		DEBUG_DXTRACE(hr);
		return false;
	}
	DEBUG_TRACE("Create vertex & index buffer successed!\n");

	DEBUG_TRACE("Init D3D successed!\n");

	m_pD3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

/*	m_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	*/
	return true;
}

void CDevice::Release()
{
	SAFE_RELEASE(m_pTextOut);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pD3DD);
	SAFE_RELEASE(m_pD3D);
}

void CDevice::BeginScene(DWORD color)
{
	m_pD3DD->Clear(0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0);
	m_pD3DD->BeginScene();
	m_dwIBOffset = 0;
	m_dwVBOffset = 0;
}

void CDevice::EndScene()
{
	FlushQuadTex();

	m_pD3DD->EndScene();
	m_pD3DD->Present(NULL, NULL, NULL, NULL);
}

void CDevice::DrawString(const char* text, const RectI& rect, DWORD format, DWORD color)
{
	RECT rc;
	rc.left = rect.left;
	rc.top = rect.top;
	rc.right = rect.right;
	rc.bottom = rect.bottom;

	m_pTextOut->DrawText(NULL, text, -1, &rc, format, color);
}

void CDevice::DrawLine(const Vec2F& pt0, const Vec2F& pt1, DWORD color)
{
	HRESULT hr = S_OK;
	stVertex* v = NULL;
	int vbSize = sizeof(stVertex) * 2;
	hr = m_pVB->Lock(m_dwVBOffset, vbSize, (void**)&v, D3DLOCK_NOOVERWRITE);
	if (FAILED(hr))
	{
		DEBUG_DXTRACE(hr);
		return;
	}

	v[0].x = pt0.x;		v[0].y = pt0.y;		v[0].z = 0.0f;	v[0].w = 1.0f;	v[0].color = color;
	v[1].x = pt1.x;		v[1].y = pt1.y;		v[1].z = 0.0f;	v[1].w = 1.0f;	v[1].color = color;
	m_pVB->Unlock();

	m_pD3DD->SetStreamSource(0, m_pVB, m_dwVBOffset, sizeof(stVertex));
	m_pD3DD->SetFVF(stVertex::FVF);
	m_pD3DD->DrawPrimitive(D3DPT_LINELIST, 0, 1);

	m_dwVBOffset += vbSize;
}

void CDevice::DrawLineList(const Vec2F* p, DWORD num, DWORD color)
{
	if (! p || num < 2)
		return;
	HRESULT hr = S_OK;
	stVertex* v = NULL;
	int lineNum = num / 2;
	int vbSize = sizeof(stVertex) * lineNum * 2;
	hr = m_pVB->Lock(m_dwVBOffset, vbSize, (void**)&v, D3DLOCK_NOOVERWRITE);
	if (FAILED(hr))
	{
		DEBUG_DXTRACE(hr);
		return;
	}

	for (int i = 0; i < lineNum; ++i)
	{
		v->x = p->x;	v->y = p->y;	v->z = 0.0f;	v->w = 1.0f;	v->color = color;
		++v;
		++p;
		v->x = p->x;	v->y = p->y;	v->z = 0.0f;	v->w = 1.0f;	v->color = color;
		++v;
		++p;
	}
	m_pVB->Unlock();

	m_pD3DD->SetStreamSource(0, m_pVB, m_dwVBOffset, sizeof(stVertex));
	m_pD3DD->SetFVF(stVertex::FVF);
	m_pD3DD->DrawPrimitive(D3DPT_LINELIST, 0, lineNum);

	m_dwVBOffset += vbSize;
}

void CDevice::DrawQuadTex(const stQuadTex& qt)
{
	auto it = m_mapQuadTex.find(qt.tex);
	if (it != m_mapQuadTex.end())
	{
		int vSize = it->second.size();
		it->second.resize(vSize + count_of(qt.vt));
		memcpy(&it->second[vSize], (void*)&qt.vt, sizeof(qt.vt));
	}
	else
	{
		m_mapQuadTex[qt.tex] = std::vector<stVertexTex>();
		std::vector<stVertexTex>& v = m_mapQuadTex[qt.tex];
		v.resize(count_of(qt.vt));
		memcpy(&v[0], (void*)&qt.vt, sizeof(qt.vt));
	}

}

void CDevice::FlushQuadTex()
{
	std::for_each(m_mapQuadTex.begin(), m_mapQuadTex.end(), [this](decltype(*m_mapQuadTex.begin())& e)
	{
		IDirect3DTexture9* tex = (IDirect3DTexture9*)e.first;

		HRESULT hr = S_OK;
		stVertexTex* vt = NULL;
		int vbNum = e.second.size();
		int vbSize = sizeof(stVertexTex) * vbNum;
		hr = m_pVB->Lock(m_dwVBOffset, vbSize, (void**)&vt, D3DLOCK_NOOVERWRITE);
		if (FAILED(hr))
		{
			DEBUG_DXTRACE(hr);
			return;
		}

		memcpy(vt, (void*)&e.second[0], vbSize);
		m_pVB->Unlock();

		WORD* w = NULL;
		int ibSize = sizeof(WORD) * 6 * vbNum / 4;
		hr = m_pIB->Lock(m_dwIBOffset, ibSize, (void**)&w, D3DLOCK_NOOVERWRITE);
		if (FAILED(hr))
		{
			DEBUG_DXTRACE(hr);
			return;
		}

		int ibNum = 6 * vbNum / 4;
		for (int i = 0, j = 0; i < ibNum; i += 6, j += 4)
		{
			w[i] = j + 3;
			w[i + 1] = j;
			w[i + 2] = j + 1;
			w[i + 3] = j + 3;
			w[i + 4] = j + 1;
			w[i + 5] = j + 2;
		}
		m_pIB->Unlock(); 

		m_pD3DD->SetTexture(0, tex);

		m_pD3DD->SetStreamSource(0, m_pVB, m_dwVBOffset, sizeof(stVertexTex));
		m_pD3DD->SetIndices(m_pIB);
		m_pD3DD->SetFVF(stVertexTex::FVF);
		m_pD3DD->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vbNum, m_dwIBOffset / sizeof(WORD), vbNum / 2);

		m_dwVBOffset += vbSize;
		m_dwIBOffset += ibSize;
	});

	m_mapQuadTex.clear();
}

void CDevice::LoadTexByFileName(const std::string& fileName)
{
	auto it = m_mapFileTex.find(fileName);
	if (it == m_mapFileTex.end() || it->second == NULL)
	{
		IDirect3DTexture9* tex = NULL;
		HRESULT hr = D3DXCreateTextureFromFile(m_pD3DD, fileName.c_str(), &tex);
		if (FAILED(hr))
		{
			DEBUG_DXTRACE(hr);
			return;
		}
		m_mapFileTex[fileName] = tex;
	}
}

IDirect3DTexture9* CDevice::GetTexByFileName(const std::string& fileName)
{
	auto it = m_mapFileTex.find(fileName);
	if (it != m_mapFileTex.end())
		return it->second;
	return NULL;
}

