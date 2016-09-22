#include "sims.h"
#include "core/log.h"
#include "graphics_api/sims_sdk_dx9.h"
#include "utils/demo_app.h"
using namespace sims;

class D3DXCreateTextApp : public DemoApp<dx9::Window>
{
public:
	D3DXCreateTextApp() : mesh_(nullptr) {}

	virtual void OnCreate()
	{
		DemoApp<dx9::Window>::OnCreate();

		HDC hdc = CreateCompatibleDC(0);
		HFONT hFont;
		HFONT hFontOld;

		LOGFONT lf;
		lf.lfHeight = 25;    // in logical units
		lf.lfWidth = 12;    // in logical units
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfWeight = 500;   // boldness, range 0(light) - 1000(bold)
		lf.lfItalic = false;
		lf.lfUnderline = false;
		lf.lfStrikeOut = false;
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfOutPrecision = 0;
		lf.lfClipPrecision = 0;
		lf.lfQuality = 0;
		lf.lfPitchAndFamily = 0;
		strncpy_s(lf.lfFaceName, "Times New Roman", sizeof(lf.lfFaceName)); // font style

		// create font and select it with device context
		hFont = CreateFontIndirect(&lf);
		hFontOld = (HFONT)SelectObject(hdc, hFont);

		// create text mesh
		dx9::CHECK_HR = D3DXCreateText(dx9::g_pD3DD,
			hdc,
			"Direct3D",
			0.001f,
			0.4f,
			&mesh_,
			0,
			0);

		// restore old font
		SelectObject(hdc, hFontOld);
		DeleteObject(hFont);
		DeleteDC(hdc);

		// setup directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = white;
		light.Specular = white;
		light.Ambient = white;
		light.Direction = D3DXVECTOR3(0.0f, -0.5f, 1.0f);

		dx9::CHECK_HR = dx9::g_pD3DD->SetLight(0, &light);
		dx9::CHECK_HR = dx9::g_pD3DD->LightEnable(0, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SPECULARENABLE, true);

		// view
		D3DXVECTOR3 pos(0.0f, 1.5f, -3.3f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &pos, &target, &up);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			0.01f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			static float y = 0.0f;
		
			D3DXMATRIX ry;
			D3DXMatrixRotationY(&ry, y);
			y += timestep.GetSeconds();
			if (y >= D3DX_PI * 2.0f)
				y = 0.0f;

			D3DXMATRIX trans;
			D3DXMatrixTranslation(&trans, -1.6f, 0.0f, 0.0f);
			D3DXMATRIX m = trans * ry;
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &m);

			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			D3DMATERIAL9 mtrl;
			GenMaterial(mtrl, white, white, white, black, 2.0f);
			dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&mtrl);
			mesh_->DrawSubset(0);

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(mesh_);
	}
private:
	ID3DXMesh* mesh_;
};

int main()
{
	D3DXCreateTextApp app;
	app.Create(640, 480, "D3DXCreateText");
	app.Loop();
	return 0;
}