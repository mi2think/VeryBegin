#include "sims.h"
#include "core/input_state.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class VSDiffuseLighting : public DemoApp<dx9::Window>
{
public:
	VSDiffuseLighting()
		: teapot_(nullptr)
		, prog_(nullptr)
		, constTable_(nullptr)
	{}

	virtual void OnCreate()
	{
		// create geometry
		dx9::CHECK_HR = D3DXCreateTeapot(dx9::g_pD3DD, &teapot_, 0);

		// compile shader
		ID3DXBuffer* shaderBuffer = nullptr;
		ID3DXBuffer* errorBuffer = nullptr;
		dx9::CHECK_HR = D3DXCompileShaderFromFile("diffuse.hlsl",
			0,
			0,
			"main",
			"vs_1_1",
#if _DEBUG
			D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION,
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

		// get handles
		viewMatrixH_ = constTable_->GetConstantByName(0, "viewM");
		viewProjMatrixH_ = constTable_->GetConstantByName(0, "viewProjM");
		ambientMtrlH_ = constTable_->GetConstantByName(0, "ambientMtrl");
		diffuseMtrlH_ = constTable_->GetConstantByName(0, "diffuseMtrl");
		lightDirH_ = constTable_->GetConstantByName(0, "lightDirection");

		// set shader constants
		D3DXVECTOR4 dirToLight(-0.57f, 0.57f, -0.57f, 0.0f);
		D3DXVec4Normalize(&dirToLight, &dirToLight);
		dx9::CHECK_HR = constTable_->SetVector(dx9::g_pD3DD, lightDirH_, &dirToLight);

		// material
		D3DXVECTOR4 ambientMtrl(0.0f, 0.0f, 1.0f, 1.0f);
		D3DXVECTOR4 diffuseMtrl(0.0f, 0.0f, 1.0f, 1.0f);
		dx9::CHECK_HR = constTable_->SetVector(dx9::g_pD3DD, ambientMtrlH_, &ambientMtrl);
		dx9::CHECK_HR = constTable_->SetVector(dx9::g_pD3DD, diffuseMtrlH_, &diffuseMtrl);
		dx9::CHECK_HR = constTable_->SetDefaults(dx9::g_pD3DD); // don't miss£¡

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

			D3DXMATRIX viewProj = view * projM_;
			// const table
			dx9::CHECK_HR = constTable_->SetMatrix(dx9::g_pD3DD, viewMatrixH_, &view);
			dx9::CHECK_HR = constTable_->SetMatrix(dx9::g_pD3DD, viewProjMatrixH_, &viewProj);

			// render
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			dx9::CHECK_HR = dx9::g_pD3DD->SetVertexShader(prog_);
			dx9::CHECK_HR = teapot_->DrawSubset(0);

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(teapot_);
		SAFE_RELEASE(prog_);
		SAFE_RELEASE(constTable_);
	}

private:
	ID3DXMesh* teapot_;
	IDirect3DVertexShader9* prog_;
	ID3DXConstantTable* constTable_;

	D3DXMATRIX projM_;

	D3DXHANDLE viewMatrixH_;
	D3DXHANDLE viewProjMatrixH_;
	D3DXHANDLE ambientMtrlH_;
	D3DXHANDLE diffuseMtrlH_;
	D3DXHANDLE lightDirH_;
};

int main()
{
	VSDiffuseLighting app;
	app.Create(640, 480, "VS Diffuse Lighting");
	app.Loop();
	return 0;
}