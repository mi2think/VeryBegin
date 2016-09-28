#include "sims.h"
#include "core/log.h"
#include "core/input_state.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class CartoonRendering : public DemoApp<dx9::Window>
{
public:
	CartoonRendering()
		: prog_(nullptr)
		, constTable_(nullptr)
		, texture_(nullptr)
	{
		memset(objs_, 0, sizeof(objs_));
	}

	virtual void OnCreate()
	{
		// create objs
		dx9::CHECK_HR = D3DXCreateTeapot(dx9::g_pD3DD, &objs_[0], 0);
		dx9::CHECK_HR = D3DXCreateSphere(dx9::g_pD3DD, 1.0f, 20, 20, &objs_[1], 0);
		dx9::CHECK_HR = D3DXCreateTorus(dx9::g_pD3DD, 0.5f, 1.0f, 20, 20, &objs_[2], 0);
		dx9::CHECK_HR = D3DXCreateCylinder(dx9::g_pD3DD, 0.5f, 0.5f, 2.0f, 20, 20, &objs_[3], 0);

		D3DXMatrixTranslation(&matrixs_[0], 0.0f, 2.0f, 0.0f);
		D3DXMatrixTranslation(&matrixs_[1], 0.0f,-2.0f, 0.0f);
		D3DXMatrixTranslation(&matrixs_[2],-3.0f, 0.0f, 0.0f);
		D3DXMatrixTranslation(&matrixs_[3], 3.0f, 0.0f, 0.0f);

		meshColors_[0] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		meshColors_[1] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		meshColors_[2] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		meshColors_[3] = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);

		// create shader
		ID3DXBuffer* shaderBuffer = nullptr;
		ID3DXBuffer* errorBuffer = nullptr;
		dx9::CHECK_HR = D3DXCompileShaderFromFile("cartoon.hlsl",
			0,
			0,
			"main",
			"vs_1_1",
#if _DEBUG
			D3DXSHADER_DEBUG,
#else
			0,
#endif
			&shaderBuffer,
			&errorBuffer,
			&constTable_);
		if (errorBuffer != nullptr)
		{
			LOG_ERROR("%s", (char*)errorBuffer->GetBufferPointer());
			SAFE_RELEASE(errorBuffer);
		}

		// create vertex shader
		dx9::CHECK_HR = dx9::g_pD3DD->CreateVertexShader((DWORD*)shaderBuffer->GetBufferPointer(), &prog_);
		SAFE_RELEASE(shaderBuffer);

		// load texture
		dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD,
			"toonshade.bmp",
			&texture_);

		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		// get handles
		WVMatrixH_ = constTable_->GetConstantByName(0, "worldViewM");
		WVPMatrixH_ = constTable_->GetConstantByName(0, "worldViewProjM");
		colorH_ = constTable_->GetConstantByName(0, "color");
		lightDirH_ = constTable_->GetConstantByName(0, "lightDir");

		// set shader constants
		D3DXVECTOR4 dirToLight(-0.57f, 0.57f, -0.57f, 0.0f);
		D3DXVec4Normalize(&dirToLight, &dirToLight);
		dx9::CHECK_HR = constTable_->SetVector(dx9::g_pD3DD, lightDirH_, &dirToLight);

		dx9::CHECK_HR = constTable_->SetDefaults(dx9::g_pD3DD);

		// projection
		D3DXMatrixPerspectiveFovLH(&projM_,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			// update scene
			static float angle = (3.0f * D3DX_PI) / 2.0f;
			static float height = 5.0f;

			float ts = timestep.GetSeconds();
			if (InputState::GetKeyState(KEY_LEFT))
				angle -= 0.5f * ts;
			else if (InputState::GetKeyState(KEY_RIGHT))
				angle += 0.5f * ts;
			else if (InputState::GetKeyState(KEY_UP))
				height += 5.0f * ts;
			else if (InputState::GetKeyState(KEY_DOWN))
				height -= 5.0f * ts;

			D3DXVECTOR3 position(cosf(angle) * 7.0f, height, sinf(angle) * 7.0f);
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
			D3DXMATRIX view;
			D3DXMatrixLookAtLH(&view, &position, &target, &up);

			// render
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			dx9::CHECK_HR = dx9::g_pD3DD->SetVertexShader(prog_);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, texture_);

			D3DXMATRIX worldView, worldViewProj;
			for (int i = 0; i < OBJ_NUM; ++i)
			{
				worldView = matrixs_[i] * view;
				worldViewProj = worldView * projM_;

				dx9::CHECK_HR = constTable_->SetMatrix(dx9::g_pD3DD, WVMatrixH_, &worldView);
				dx9::CHECK_HR = constTable_->SetMatrix(dx9::g_pD3DD, WVPMatrixH_, &worldViewProj);
				dx9::CHECK_HR = constTable_->SetVector(dx9::g_pD3DD, colorH_, &meshColors_[i]);

				dx9::CHECK_HR = objs_[i]->DrawSubset(0);
			}

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(texture_);
		SAFE_RELEASE(prog_);
		SAFE_RELEASE(constTable_);
		for (auto& obj : objs_)
		{
			SAFE_RELEASE(obj);
		}
	}

	enum { OBJ_NUM = 4 };
private:
	ID3DXMesh* objs_[OBJ_NUM];
	D3DXMATRIX matrixs_[OBJ_NUM];
	D3DXVECTOR4 meshColors_[OBJ_NUM];

	D3DXMATRIX projM_;

	IDirect3DVertexShader9* prog_;
	ID3DXConstantTable* constTable_;
	IDirect3DTexture9* texture_;

	D3DXHANDLE WVMatrixH_;
	D3DXHANDLE WVPMatrixH_;
	D3DXHANDLE colorH_;
	D3DXHANDLE lightDirH_;
};

int main()
{
	CartoonRendering app;
	app.Create(640, 480, "Cartoon Rendering");
	app.Loop();
	return 0;
}