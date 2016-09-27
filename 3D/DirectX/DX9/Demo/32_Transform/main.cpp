#include "sims.h"
#include "core/log.h"
#include "core/input_state.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class Transform : public DemoApp<dx9::Window>
{
public:
	Transform()
		: prog_(nullptr)
		, constTable_(nullptr)
		, teapot_(nullptr)
	{}

	virtual void OnCreate()
	{
		// create geometry
		dx9::CHECK_HR = D3DXCreateTeapot(dx9::g_pD3DD,
			&teapot_,
			nullptr);

		// compile shader
		ID3DXBuffer* shaderBuffer = nullptr;
		ID3DXBuffer* errorBuffer = nullptr;
		dx9::CHECK_HR = D3DXCompileShaderFromFile("transform.hlsl",
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

		// get handles
		handle_ = constTable_->GetConstantByName(0, "viewProjM");

		// set projection matrix
		D3DXMatrixPerspectiveFovLH(&projM_,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);

		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
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
				height += 0.5f * ts;
			else if (InputState::GetKeyState(KEY_DOWN))
				height -= 0.5f * ts;

			D3DXVECTOR3 position(cosf(angle) * 10.0f, height, sinf(angle) * 10.0f);
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
			D3DXMATRIX view;
			D3DXMatrixLookAtLH(&view, &position, &target, &up);

			D3DXMATRIX viewProj = view * projM_;
			constTable_->SetMatrix(dx9::g_pD3DD, handle_, &viewProj);

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
		SAFE_RELEASE(prog_);
		SAFE_RELEASE(constTable_);
		SAFE_RELEASE(teapot_);
	}

private:
	IDirect3DVertexShader9* prog_;
	ID3DXConstantTable* constTable_;
	ID3DXMesh* teapot_;
	
	D3DXHANDLE handle_;
	D3DXMATRIX projM_;
};

int main()
{
	Transform app;
	app.Create(640, 480, "Transform");
	app.Loop();
	return 0;
}