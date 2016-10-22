#include "sims.h"
#include "core/log.h"
#include "core/input_state.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_d3d9.h"
using namespace sims;

class CartoonEffect : public DemoApp<d3d9::Window>
{
public:
	CartoonEffect()
		: tech_(nullptr)
		, texture_(nullptr)
	{
		memset(objs_, 0, sizeof(objs_));
	}

	virtual void OnCreate()
	{
		// create objs
		d3d9::CHECK_HR = D3DXCreateTeapot(d3d9::g_pD3DD, &objs_[0], 0);
		d3d9::CHECK_HR = D3DXCreateSphere(d3d9::g_pD3DD, 1.0f, 20, 20, &objs_[1], 0);
		d3d9::CHECK_HR = D3DXCreateTorus(d3d9::g_pD3DD, 0.5f, 1.0f, 20, 20, &objs_[2], 0);
		d3d9::CHECK_HR = D3DXCreateCylinder(d3d9::g_pD3DD, 0.5f, 0.5f, 2.0f, 20, 20, &objs_[3], 0);

		D3DXMatrixTranslation(&matrixs_[0], 0.0f, 2.0f, 0.0f);
		D3DXMatrixTranslation(&matrixs_[1], 0.0f, -2.0f, 0.0f);
		D3DXMatrixTranslation(&matrixs_[2], -3.0f, 0.0f, 0.0f);
		D3DXMatrixTranslation(&matrixs_[3], 3.0f, 0.0f, 0.0f);

		meshColors_[0] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		meshColors_[1] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		meshColors_[2] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		meshColors_[3] = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);

		// create effects
		ID3DXBuffer* errorBuffer = nullptr;
		d3d9::CHECK_HR = D3DXCreateEffectFromFile(d3d9::g_pD3DD,
			"cartoon.fx",
			0,
			0,
			D3DXSHADER_DEBUG,
			0,
			&tech_,
			&errorBuffer);
		if (errorBuffer != nullptr)
		{
			LOG_ERROR("%s", (char*)errorBuffer->GetBufferPointer());
			SAFE_RELEASE(errorBuffer);
		}

		// save handles
		WVMatrixH_ = tech_->GetParameterByName(0, "worldViewM");
		WVPMatrixH_ = tech_->GetParameterByName(0, "worldViewProjM");
		colorH_ = tech_->GetParameterByName(0, "color");
		lightDirH_ = tech_->GetParameterByName(0, "lightDir");
		texH_ = tech_->GetParameterByName(0, "tex");
		techH_ = tech_->GetTechniqueByName("Cartoon");

		// set effect parameters
		D3DXMatrixPerspectiveFovLH(&projM_,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);

		D3DXVECTOR4 lightDir(-0.57f, 0.57f, -0.57f, 0.0f);
		d3d9::CHECK_HR = tech_->SetVector(lightDirH_, &lightDir);

		d3d9::CHECK_HR = D3DXCreateTextureFromFile(d3d9::g_pD3DD,
			"toonshade.bmp",
			&texture_);
		d3d9::CHECK_HR = tech_->SetTexture(texH_, texture_);
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

			D3DXVECTOR3 position(cosf(angle) * 10.0f, height, sinf(angle) * 10.0f);
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
			D3DXMATRIX view;
			D3DXMatrixLookAtLH(&view, &position, &target, &up);

			// active technique and render
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			d3d9::CHECK_HR = tech_->SetTechnique(techH_);

			UINT numPasses = 0;
			d3d9::CHECK_HR = tech_->Begin(&numPasses, 0);

			D3DXMATRIX worldView, worldViewProj;
			for (int n = 0; n < numPasses; ++n)
			{
				d3d9::CHECK_HR = tech_->BeginPass(n);
				for (int i = 0; i < OBJ_NUM; ++i)
				{
					worldView = matrixs_[i] * view;
					worldViewProj = worldView * projM_;

					d3d9::CHECK_HR = tech_->SetMatrix(WVMatrixH_, &worldView);
					d3d9::CHECK_HR = tech_->SetMatrix(WVPMatrixH_, &worldViewProj);
					d3d9::CHECK_HR = tech_->SetVector(colorH_, &meshColors_[i]);

					d3d9::CHECK_HR = tech_->CommitChanges();

					d3d9::CHECK_HR = objs_[i]->DrawSubset(0);
				}
				d3d9::CHECK_HR = tech_->EndPass();
			}
			d3d9::CHECK_HR = tech_->End();

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(texture_);
		SAFE_RELEASE(tech_);
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

	ID3DXEffect* tech_;
	IDirect3DTexture9* texture_;

	D3DXHANDLE WVMatrixH_;
	D3DXHANDLE WVPMatrixH_;
	D3DXHANDLE colorH_;
	D3DXHANDLE lightDirH_;
	D3DXHANDLE texH_;
	D3DXHANDLE techH_;
};

int main()
{
	CartoonEffect app;
	app.Create(640, 480, "Cartoon Effect");
	app.Loop();
	return 0;
}