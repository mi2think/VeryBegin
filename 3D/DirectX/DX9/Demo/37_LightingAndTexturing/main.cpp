#include "sims.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "core/log.h"
#include "core/input_state.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class LightingAndTexturing : public DemoApp<dx9::Window>
{
public:
	LightingAndTexturing()
		: mesh_(nullptr)
		, lightTexEffect_(nullptr)
	{
	}

	virtual void OnCreate()
	{
		// load x file data
		ID3DXBuffer* mtrlBuffer = nullptr;
		DWORD numMtrls = 0;

		dx9::CHECK_HR = D3DXLoadMeshFromX(
			"mountain.x",
			D3DXMESH_MANAGED,
			dx9::g_pD3DD,
			0,
			&mtrlBuffer,
			0,
			&numMtrls,
			&mesh_);

		// extract the materials, load textures
		if (mtrlBuffer != nullptr && numMtrls != 0)
		{
			D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();

			for (int i = 0; i < numMtrls; ++i)
			{
				mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

				materials_.push_back(mtrls[i].MatD3D);

				IDirect3DTexture9* tex = nullptr;
				if (mtrls[i].pTextureFilename != nullptr)
				{
					dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD,
						mtrls[i].pTextureFilename,
						&tex);
				}
				textures_.push_back(tex);
			}
		}
		SAFE_RELEASE(mtrlBuffer);

		// create effect
		ID3DXBuffer* errorBuffer = nullptr;
		dx9::CHECK_HR = D3DXCreateEffectFromFile(dx9::g_pD3DD,
			"light_tex.fx",
			0,
			0,
#ifdef _DEBUG
			D3DXSHADER_DEBUG,
#else
			0,
#endif
			0,
			&lightTexEffect_,
			&errorBuffer);

		if (errorBuffer)
		{
			LOG_ERROR("%s", (char*)errorBuffer->GetBufferPointer());
			SAFE_RELEASE(errorBuffer);
		}

		// save frequently accessed parameter handles
		worldMatrixH_ = lightTexEffect_->GetParameterByName(0, "worldM");
		viewMatrixH_ = lightTexEffect_->GetParameterByName(0, "viewM");
		projMatrixH_ = lightTexEffect_->GetParameterByName(0, "projM");
		texH_ = lightTexEffect_->GetParameterByName(0, "tex");

		lightTexTechH_ = lightTexEffect_->GetTechniqueByName("lightAndTexture");

		// set effect parameters

		// matrix
		D3DXMATRIX W, P;
		D3DXMatrixIdentity(&W);
		lightTexEffect_->SetMatrix(worldMatrixH_, &W);

		D3DXMatrixPerspectiveFovLH(&P,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);

		lightTexEffect_->SetMatrix(projMatrixH_, &P);

		// texture
		IDirect3DTexture9* tex = nullptr;
		dx9::CHECK_HR = D3DXCreateTextureFromFile(dx9::g_pD3DD, "Terrain_3x_diffcol.jpg", &tex);
		lightTexEffect_->SetTexture(texH_, tex);
		SAFE_RELEASE(tex);
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

			D3DXVECTOR3 position(cosf(angle) * 10.0f, height, sinf(angle) * 10.0f);
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
			D3DXMATRIX view;
			D3DXMatrixLookAtLH(&view, &position, &target, &up);

			lightTexEffect_->SetMatrix(viewMatrixH_, &view);

			// active the technique and render
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			// set technique to use
			dx9::CHECK_HR = lightTexEffect_->SetTechnique(lightTexTechH_);
			UINT numPasses = 0;
			dx9::CHECK_HR = lightTexEffect_->Begin(&numPasses, 0);

			for (int i = 0; i < numPasses; ++i)
			{
				lightTexEffect_->BeginPass(i);
				for (int j = 0; j < materials_.size(); ++j)
				{
					mesh_->DrawSubset(j);
				}
				lightTexEffect_->EndPass();
			}

			dx9::CHECK_HR = lightTexEffect_->End();

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}


	virtual void OnDestroy()
	{
		SAFE_RELEASE(mesh_);
		SAFE_RELEASE(lightTexEffect_);
		for (auto& tex : textures_)
		{
			SAFE_RELEASE(tex);
		}
	}

private:
	ID3DXMesh* mesh_;
	vector<D3DMATERIAL9> materials_;
	vector<IDirect3DTexture9*> textures_;
	ID3DXEffect* lightTexEffect_;

	D3DXHANDLE worldMatrixH_;
	D3DXHANDLE viewMatrixH_;
	D3DXHANDLE projMatrixH_;
	D3DXHANDLE texH_;

	D3DXHANDLE lightTexTechH_;
};

int main()
{
	LightingAndTexturing app;
	app.Create(640, 480, "Lighting And Texturing");
	app.Loop();
	return 0;
}