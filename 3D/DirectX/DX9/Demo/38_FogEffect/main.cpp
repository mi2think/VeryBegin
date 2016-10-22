#include "sims.h"
#include "core/log.h"
#include "math/vector3.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "camera.h"
#include "terrain.h"
using namespace sims;

class FogEffect : public DemoApp<d3d9::Window>
{
public:
	FogEffect()
		: terrain_(nullptr)
		, fogTech_(nullptr)
	{
	}

	virtual void OnCreate()
	{
		// init scene
		terrain_ = new Terrain(d3d9::g_pD3DD,
			"coastMountain64.raw",
			0.5f,
			640,
			640,
			64,
			64);
		terrain_->GenerateTexture(Vector3f(0.0f, -1.0f, 0.0f));

		// set texture filters
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// eye
		camera_.SetEye(Vector3f(100.0f, 100.0f, -250.0f));

		// create effect
		ID3DXBuffer* errorBuffer = nullptr;
		d3d9::CHECK_HR = D3DXCreateEffectFromFile(d3d9::g_pD3DD,
			"fog.fx",
			0,
			0,
#ifdef _DEBUG
			D3DXSHADER_DEBUG,
#else
			0,
#endif
			0,
			&fogTech_,
			&errorBuffer);
		if (errorBuffer)
		{
			LOG_ERROR("%s", (char*)errorBuffer->GetBufferPointer());
			SAFE_RELEASE(errorBuffer);
		}

		// save handles
		fogTechH_ = fogTech_->GetTechniqueByName("Fog");

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0,
			1000.0f);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnUpdate(const Timestep& timestep)
	{
		camera_.OnUpdate(timestep);
	}

	virtual void OnRender(const Timestep&)
	{
		if (d3d9::g_pD3DD)
		{
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			D3DXMATRIX view;
			camera_.GetViewMatrix(&view);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

			// set technique to use
			fogTech_->SetTechnique(fogTechH_);

			UINT numPasses = 0;
			d3d9::CHECK_HR = fogTech_->Begin(&numPasses, 0);
			D3DXMATRIX I;
			D3DXMatrixIdentity(&I);
			for (int i = 0; i < numPasses; ++i)
			{
				fogTech_->BeginPass(i);
				if (terrain_)
					terrain_->Draw(I, false);
				fogTech_->EndPass();
			}
			d3d9::CHECK_HR = fogTech_->End();

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_DELETE(terrain_);
		SAFE_RELEASE(fogTech_);
	}

private:
	Camera camera_;
	Terrain* terrain_;
	ID3DXEffect* fogTech_;
	D3DXHANDLE fogTechH_;
};

int main()
{
	FogEffect app;
	app.Create(640, 480, "Fog Effect");
	app.Loop();
	return 0;
}