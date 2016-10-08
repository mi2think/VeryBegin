#include "sims.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "core/color.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class Textures : public DemoApp<dx9::Window>
{
public:
	struct CUSTOMVERTEX
	{
		Vector3f position;
		Color color;
#ifdef SHOW_HOW_TO_USE_TCI
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
#else
		Vector2f uv;
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
#endif

	};


	Textures()
		: vb_(nullptr)
		, tex_(nullptr)
	{}

	virtual void OnCreate()
	{
		// create vertex buffer
		dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexBuffer(50 * 2 * sizeof(CUSTOMVERTEX),
			D3DUSAGE_WRITEONLY,
			CUSTOMVERTEX::FVF,
			D3DPOOL_DEFAULT,
			&vb_,
			0);

		// fill vertex buffer. we are algorithmically generating a cylinder here,
		// including the normals, which are used for lighting
		CUSTOMVERTEX* v = nullptr;
		dx9::CHECK_HR = vb_->Lock(0, 0, (void**)&v, 0);
		for (int i = 0; i < 50; ++i)
		{
			float delta = (2 * D3DX_PI * i) / (50 - 1);
			v[2 * i].position = Vector3f(sinf(delta), -1.0f, cosf(delta));
			v[2 * i].color = Color(0xffffffff);
			v[2 * i + 1].position = Vector3f(sinf(delta), 1.0f, cosf(delta));
			v[2 * i + 1].color = Color(0xff808080);
#ifndef SHOW_HOW_TO_USE_TCI
			v[2 * i].uv = Vector2f((float)i / (50 - 1), 1.0f);
			v[2 * i + 1].uv = Vector2f((float)i / (50 - 1), 0.0f);
#endif
		}
		dx9::CHECK_HR = vb_->Unlock();

		// load texture
		dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD,
			"banana.bmp",
			&tex_);

		// view
		D3DXVECTOR3 pos(0.0f, 3.0f, -5.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &pos, &target, &up);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI / 4.0f,
			(float)width_ / height_,
			1.0f,
			100.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);

		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, false);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ZENABLE, true);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			static float x = 0.0f;

			x += timestep.GetSeconds();
			if (x >= 2 * D3DX_PI)
				x = 0.0f;

			// world
			D3DXMATRIX world;
			D3DXMatrixRotationX(&world, x);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &world);

			// draw

			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff0000ff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			// setup our texture. using textures introduces the texture stage states,
			// which govern how textures get blended together(in the case of multiple textures)
			// and lighting information. in this case, we are modulating(blending) our texture with
			// diffuse color of vertices.
			dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, tex_);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

#ifdef SHOW_HOW_TO_USE_TCI
			// note: to use D3D texture coordinate generation, use the stage state D3DTSS_TEXCOORDINDEX, as shown below.
			// in this example, we are using the position of the vertex in camera space(D3DTSS_TCI_CAMERASPACEPOSITION)
			// to generate texture coordinates. Camera space is the vertex position multiplied by the world and view matrix
			// the tex coord index(TCI) parameters are passed into a texture transform, which is a 4x4 matrix which transform
			// the x, y, z TCI coordinates into u, v texture coordinates.

			// in this example, the texture matrix is setup to transform the input camera space coordinates(all of R^3) to 
			// projection space(-1, +1) and finally to texture space(0, 1).
			//    CameraSpace.xyzw = (input vertex position) * (WorldView)
			//    ProjSpace.xyzw =  CameraSpace.xyzw * Projection  // move to -1 to 1
			//    TexSpace.xyzw = ProjSpace.xyzw * (0.5, -0.5, 1.0, 1.0) // scale to -0.5 to 0.5(flip y)
			//    TexSpace.xyzw += (0.5, 0.5, 0.0, 0.0) // shift to 0 to 1

			// setting D3DTSS_TCI_CAMERASPACEPOSITION to D3DTTFF_COUNT4 | D3DTTFF_PROJECTED tells D3D to divide
			// the input texture coordinates by the 4th(w) component.
			// this divide is necessary when performing a perspective projection since the TexSpace.xy coordinates
			// prior to the homogeneous divide are not actually in the 0 to 1 range
			D3DXMATRIX textureTransform;
			D3DXMATRIX proj;
			D3DXMATRIX trans;
			D3DXMATRIX scale;

			dx9::CHECK_HR = dx9::g_pD3DD->GetTransform(D3DTS_PROJECTION, &proj);
			D3DXMatrixTranslation(&trans, 0.5f, 0.5f, 0.0f);
			D3DXMatrixScaling(&scale, 0.5f, -0.5f, 1.0f);
			textureTransform = proj * scale * trans;

			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_TEXTURE0, &textureTransform);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4 | D3DTTFF_PROJECTED);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
#endif

			dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, vb_, 0, sizeof(CUSTOMVERTEX));
			dx9::CHECK_HR = dx9::g_pD3DD->SetFVF(CUSTOMVERTEX::FVF);
			dx9::CHECK_HR = dx9::g_pD3DD->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * (50 - 1));

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(vb_);
		SAFE_RELEASE(tex_);
	}
private:
	IDirect3DVertexBuffer9* vb_;
	IDirect3DTexture9* tex_;
};

int main()
{
	Textures app;
	app.Create(300, 300, "Textures");
	app.Loop();
	return 0;
}
