#include "sims.h"
#include "core/log.h"
#include "core/input_state.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "silhouette_edges.h"
using namespace sims;

class CartoonRenderingOutline : public DemoApp<d3d9::Window>
{
public:
	CartoonRenderingOutline()
		: prog_(nullptr)
		, constTable_(nullptr)
		, progOutline_(nullptr)
		, constTableOutline_(nullptr)
		, texture_(nullptr)
	{
		memset(objs_, 0, sizeof(objs_));
	}

	virtual void OnCreate()
	{
		ID3DXBuffer* adjBuffer[OBJ_NUM] = { 0, 0, 0, 0 };

		// create objs
		d3d9::CHECK_HR = D3DXCreateTeapot(d3d9::g_pD3DD, &objs_[0], &adjBuffer[0]);
		d3d9::CHECK_HR = D3DXCreateSphere(d3d9::g_pD3DD, 1.0f, 20, 20, &objs_[1], &adjBuffer[1]);
		d3d9::CHECK_HR = D3DXCreateTorus(d3d9::g_pD3DD, 0.5f, 1.0f, 20, 20, &objs_[2], &adjBuffer[2]);
		d3d9::CHECK_HR = D3DXCreateCylinder(d3d9::g_pD3DD, 0.5f, 0.5f, 2.0f, 20, 20, &objs_[3], &adjBuffer[3]);

		D3DXMatrixTranslation(&matrixs_[0], 0.0f, 2.0f, 0.0f);
		D3DXMatrixTranslation(&matrixs_[1], 0.0f, -2.0f, 0.0f);
		D3DXMatrixTranslation(&matrixs_[2], -3.0f, 0.0f, 0.0f);
		D3DXMatrixTranslation(&matrixs_[3], 3.0f, 0.0f, 0.0f);

		meshColors_[0] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		meshColors_[1] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		meshColors_[2] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		meshColors_[3] = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);

		// allocate mesh outlines
		outlines_[0] = new SilhouetteEdges(d3d9::g_pD3DD, objs_[0], adjBuffer[0]);
		outlines_[1] = new SilhouetteEdges(d3d9::g_pD3DD, objs_[1], adjBuffer[1]);
		outlines_[2] = new SilhouetteEdges(d3d9::g_pD3DD, objs_[2], adjBuffer[2]);
		outlines_[3] = new SilhouetteEdges(d3d9::g_pD3DD, objs_[3], adjBuffer[3]);

		for (auto& buffer : adjBuffer)
		{
			SAFE_RELEASE(buffer);
		}

		// create cartoon shader
		ID3DXBuffer* shaderBuffer = nullptr;
		ID3DXBuffer* errorBuffer = nullptr;
		d3d9::CHECK_HR = D3DXCompileShaderFromFile("cartoon.hlsl",
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
		d3d9::CHECK_HR = d3d9::g_pD3DD->CreateVertexShader((DWORD*)shaderBuffer->GetBufferPointer(), &prog_);
		SAFE_RELEASE(shaderBuffer);

		// create outline shader
		d3d9::CHECK_HR = D3DXCompileShaderFromFile("outline.hlsl",
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
			&constTableOutline_);
		if (errorBuffer != nullptr)
		{
			LOG_ERROR("%s", (char*)errorBuffer->GetBufferPointer());
			SAFE_RELEASE(errorBuffer);
		}
		d3d9::CHECK_HR = d3d9::g_pD3DD->CreateVertexShader((DWORD*)shaderBuffer->GetBufferPointer(), &progOutline_);
		SAFE_RELEASE(shaderBuffer);


		// load texture
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD,
			"toonshade.bmp",
			&texture_);

		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		// get handles
		WVMatrixH_ = constTable_->GetConstantByName(0, "worldViewM");
		WVPMatrixH_ = constTable_->GetConstantByName(0, "worldViewProjM");
		colorH_ = constTable_->GetConstantByName(0, "color");
		lightDirH_ = constTable_->GetConstantByName(0, "lightDir");

		outlineWVMatrixH_ = constTableOutline_->GetConstantByName(0, "worldViewM");
		outlineProjMatrixH_ = constTableOutline_->GetConstantByName(0, "projM");

		// set shader constants
		D3DXVECTOR4 dirToLight(-0.57f, 0.57f, -0.57f, 0.0f);
		D3DXVec4Normalize(&dirToLight, &dirToLight);
		d3d9::CHECK_HR = constTable_->SetVector(d3d9::g_pD3DD, lightDirH_, &dirToLight);

		d3d9::CHECK_HR = constTable_->SetDefaults(d3d9::g_pD3DD);

		// projection
		D3DXMatrixPerspectiveFovLH(&projM_,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (d3d9::g_pD3DD)
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
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			// draw cartoon
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetVertexShader(prog_);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, texture_);

			D3DXMATRIX worldView, worldViewProj;
			for (int i = 0; i < OBJ_NUM; ++i)
			{
				worldView = matrixs_[i] * view;
				worldViewProj = worldView * projM_;

				d3d9::CHECK_HR = constTable_->SetMatrix(d3d9::g_pD3DD, WVMatrixH_, &worldView);
				d3d9::CHECK_HR = constTable_->SetMatrix(d3d9::g_pD3DD, WVPMatrixH_, &worldViewProj);
				d3d9::CHECK_HR = constTable_->SetVector(d3d9::g_pD3DD, colorH_, &meshColors_[i]);

				d3d9::CHECK_HR = objs_[i]->DrawSubset(0);
			}

			// draw outline
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

			d3d9::CHECK_HR = d3d9::g_pD3DD->SetVertexShader(progOutline_);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTexture(0, 0);
			for (int i = 0; i < OBJ_NUM; ++i)
			{
				worldView = matrixs_[i] * view;

				d3d9::CHECK_HR = constTableOutline_->SetMatrix(d3d9::g_pD3DD, outlineWVMatrixH_, &worldView);
				d3d9::CHECK_HR = constTableOutline_->SetMatrix(d3d9::g_pD3DD, outlineProjMatrixH_, &projM_);

				outlines_[i]->Render();
			}

			d3d9::CHECK_HR = d3d9::g_pD3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(texture_);
		SAFE_RELEASE(prog_);
		SAFE_RELEASE(constTable_);
		SAFE_RELEASE(progOutline_);
		SAFE_RELEASE(constTableOutline_);
		for (auto& obj : objs_)
		{
			SAFE_RELEASE(obj);
		}
		for (auto& outline : outlines_)
		{
			SAFE_DELETE(outline);
		}
	}

	enum { OBJ_NUM = 4 };
private:
	ID3DXMesh* objs_[OBJ_NUM];
	D3DXMATRIX matrixs_[OBJ_NUM];
	D3DXVECTOR4 meshColors_[OBJ_NUM];
	SilhouetteEdges* outlines_[OBJ_NUM];

	D3DXMATRIX projM_;

	IDirect3DVertexShader9* prog_;
	IDirect3DVertexShader9* progOutline_;

	ID3DXConstantTable* constTable_;
	ID3DXConstantTable* constTableOutline_;
	IDirect3DTexture9* texture_;

	D3DXHANDLE WVMatrixH_;
	D3DXHANDLE WVPMatrixH_;
	D3DXHANDLE colorH_;
	D3DXHANDLE lightDirH_;

	D3DXHANDLE outlineWVMatrixH_;
	D3DXHANDLE outlineProjMatrixH_;
};

int main()
{
	CartoonRenderingOutline app;
	app.Create(640, 480, "Cartoon Rendering Outline");
	app.Loop();
	return 0;
}