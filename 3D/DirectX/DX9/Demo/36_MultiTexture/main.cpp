#include "sims.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "core/log.h"
#include "core/input_state.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

struct MultiTexVertex
{
	Vector3f p;
	Vector2f uv0;
	Vector2f uv1;
	Vector2f uv2;

	MultiTexVertex(const Vector3f& _p,
		const Vector2f& _uv0,
		const Vector2f& _uv1,
		const Vector2f& _uv2)
		: p(_p)
		, uv0(_uv0)
		, uv1(_uv1)
		, uv2(_uv2)
	{}
};

class MultiTexture : public DemoApp<dx9::Window>
{
public:
	MultiTexture()
		: quadVB_(nullptr)
		, progPS_(nullptr)
		, multiTexCT_(nullptr)
		, baseTex_(nullptr)
		, spotLightTex_(nullptr)
		, textTex_(nullptr)
	{}

	virtual void OnCreate()
	{
		// create quad
		dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexBuffer(6 * sizeof(MultiTexVertex),
			D3DUSAGE_WRITEONLY,
			0, // using vertex decl
			D3DPOOL_MANAGED,
			&quadVB_,
			0);
		MultiTexVertex* v = nullptr;
		dx9::CHECK_HR = quadVB_->Lock(0, 0, (void**)&v, 0);

		v[0] = MultiTexVertex(Vector3f(-10.0f, -10.0f, 5.0f), Vector2f(0.0f, 1.0f), Vector2f(0.0f, 1.0f), Vector2f(0.0f, 1.0f));
		v[1] = MultiTexVertex(Vector3f(-10.0f,  10.0f, 5.0f), Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f));
		v[2] = MultiTexVertex(Vector3f( 10.0f,  10.0f, 5.0f), Vector2f(1.0f, 0.0f), Vector2f(1.0f, 0.0f), Vector2f(1.0f, 0.0f));

		v[3] = MultiTexVertex(Vector3f(-10.0f, -10.0f, 5.0f), Vector2f(0.0f, 1.0f), Vector2f(0.0f, 1.0f), Vector2f(0.0f, 1.0f));
		v[4] = MultiTexVertex(Vector3f( 10.0f,  10.0f, 5.0f), Vector2f(1.0f, 0.0f), Vector2f(1.0f, 0.0f), Vector2f(1.0f, 0.0f));
		v[5] = MultiTexVertex(Vector3f( 10.0f, -10.0f, 5.0f), Vector2f(1.0f, 1.0f), Vector2f(1.0f, 1.0f), Vector2f(1.0f, 1.0f));

		dx9::CHECK_HR = quadVB_->Unlock();

		// create vertex decl
		D3DVERTEXELEMENT9 decl[] =
		{
			{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
			{0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
			{0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
			D3DDECL_END()
		};
		dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexDeclaration(decl, &decl_);

		// compile shader
		ID3DXBuffer* shaderBuffer = nullptr;
		ID3DXBuffer* errorBuffer = nullptr;
		dx9::CHECK_HR = D3DXCompileShaderFromFile(
			"multi_texture.hlsl",
			0,
			0,
			"main",
			"ps_2_0",
#if _DEBUG
			D3DXSHADER_DEBUG,
#else
			0,
#endif
			&shaderBuffer,
			&errorBuffer,
			&multiTexCT_);
		if (errorBuffer != nullptr)
		{
			LOG_ERROR("%s", (char*)errorBuffer->GetBufferPointer());
			SAFE_RELEASE(errorBuffer);
		}

		// create pixel shader
		dx9::CHECK_HR = dx9::g_pD3DD->CreatePixelShader((DWORD*)shaderBuffer->GetBufferPointer(), &progPS_);
		SAFE_RELEASE(shaderBuffer);

		// load textures
		dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD, "crate.bmp", &baseTex_);
		dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD, "spotlight.bmp", &spotLightTex_);
		dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD, "text.bmp", &textTex_);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);

		// disable lighting
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, false);

		// get handles
		baseTexH_ = multiTexCT_->GetConstantByName(0, "baseTex");
		spotLightTexH_ = multiTexCT_->GetConstantByName(0, "spotLightTex");
		textTexH_ = multiTexCT_->GetConstantByName(0, "textTex");

		// set constant desc
		uint32 count = 0;
		dx9::CHECK_HR = multiTexCT_->GetConstantDesc(baseTexH_, &baseTexDesc_, &count);
		dx9::CHECK_HR = multiTexCT_->GetConstantDesc(spotLightTexH_, &spotLightTexDesc_, &count);
		dx9::CHECK_HR = multiTexCT_->GetConstantDesc(textTexH_, &textTexDesc_, &count);

		dx9::CHECK_HR = multiTexCT_->SetDefaults(dx9::g_pD3DD);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			// update
			static float angle = 3.0f * D3DX_PI / 2.0f;
			static float radius = 20.0f;

			float ts = timestep.GetSeconds();
			if (InputState::GetKeyState(KEY_LEFT))
				angle -= 0.5f * ts;
			if (InputState::GetKeyState(KEY_RIGHT))
				angle += 0.5f * ts;
			if (InputState::GetKeyState(KEY_UP))
				radius -= 2.0f * ts;
			if (InputState::GetKeyState(KEY_DOWN))
				radius += 2.0f * ts;

			D3DXVECTOR3 position(cosf(angle) * radius, 0.0f, sinf(angle) * radius);
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
			D3DXMATRIX view;
			D3DXMatrixLookAtLH(&view, &position, &target, &up);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

			// render
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			dx9::CHECK_HR = dx9::g_pD3DD->SetPixelShader(progPS_);
			dx9::CHECK_HR = dx9::g_pD3DD->SetVertexDeclaration(decl_);
			dx9::CHECK_HR = dx9::g_pD3DD->SetStreamSource(0, quadVB_, 0, sizeof(MultiTexVertex));

			// base tex
			dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(baseTexDesc_.RegisterIndex, baseTex_);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(baseTexDesc_.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(baseTexDesc_.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(baseTexDesc_.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

			// spotlight tex
			dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(spotLightTexDesc_.RegisterIndex, spotLightTex_);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(spotLightTexDesc_.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(spotLightTexDesc_.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(spotLightTexDesc_.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

			// text tex
			dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(textTexDesc_.RegisterIndex, textTex_);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(textTexDesc_.RegisterIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(textTexDesc_.RegisterIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(textTexDesc_.RegisterIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

			dx9::CHECK_HR = dx9::g_pD3DD->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(quadVB_);
		SAFE_RELEASE(decl_);
		SAFE_RELEASE(progPS_);
		SAFE_RELEASE(multiTexCT_);
		SAFE_RELEASE(baseTex_);
		SAFE_RELEASE(spotLightTex_);
		SAFE_RELEASE(textTex_);
	}
private:
	IDirect3DVertexBuffer9* quadVB_;
	IDirect3DVertexDeclaration9* decl_;
	IDirect3DPixelShader9* progPS_;
	ID3DXConstantTable* multiTexCT_;

	IDirect3DTexture9* baseTex_;
	IDirect3DTexture9* spotLightTex_;
	IDirect3DTexture9* textTex_;

	D3DXHANDLE baseTexH_;
	D3DXHANDLE spotLightTexH_;
	D3DXHANDLE textTexH_;

	D3DXCONSTANT_DESC baseTexDesc_;
	D3DXCONSTANT_DESC spotLightTexDesc_;
	D3DXCONSTANT_DESC textTexDesc_;
};

int main()
{
	MultiTexture app;
	app.Create(640, 480, "Multi Texture");
	app.Loop();
	return 0;
}
