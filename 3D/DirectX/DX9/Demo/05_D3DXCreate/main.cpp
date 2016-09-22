#include "sims.h"
#include "core/log.h"
#include "graphics_api/sims_sdk_dx9.h"
#include "utils/demo_app.h"
using namespace sims;

class D3DXCreate : public DemoApp<dx9::Window>
{
public:
	D3DXCreate()
	{
		memset(objs_, 0, sizeof(objs_));
	}

	virtual void OnCreate()
	{
		DemoApp<dx9::Window>::OnCreate();

		// create obj
		dx9::CHECK_HR = D3DXCreateTeapot(dx9::g_pD3DD, &objs_[0], 0);
		dx9::CHECK_HR = D3DXCreateBox(dx9::g_pD3DD, 2.0f, 2.0f, 2.0f, &objs_[1], 0);
		dx9::CHECK_HR = D3DXCreateCylinder(dx9::g_pD3DD, 1.0f, 1.0f, 3.0f, 10, 10, &objs_[2], 0);
		dx9::CHECK_HR = D3DXCreateTorus(dx9::g_pD3DD, 1.0f, 3.0f, 10, 10, &objs_[3], 0);
		dx9::CHECK_HR = D3DXCreateSphere(dx9::g_pD3DD, 1.0f, 10, 10, &objs_[4], 0);

		// build matrix
		D3DXMatrixTranslation(&matrixs_[0], 0.0f, 0.0f, 0.0f);
		D3DXMatrixTranslation(&matrixs_[1],-5.0f, 0.0f, 5.0f);
		D3DXMatrixTranslation(&matrixs_[2], 5.0f, 0.0f, 5.0f);
		D3DXMatrixTranslation(&matrixs_[3],-5.0f, 0.0f,-5.0f);
		D3DXMatrixTranslation(&matrixs_[4], 5.0f, 0.0f,-5.0f);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.5f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);

		// wireframe
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			// animate camera:
			// camera will circle around the center of scene
			static float angle = 3.0f * D3DX_PI / 2.0f;
			static float cameraH = 0.0f;
			static float cameraHOffset = 5.0f;

			D3DXVECTOR3 position(cosf(angle) * 10.0f, cameraH, sinf(angle) * 10.0f);
			D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
			D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
			D3DXMATRIX view;
			D3DXMatrixLookAtLH(&view, &position, &target, &up);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);
			
			angle += timestep.GetSeconds();
			if (angle >= 2 * D3DX_PI)
				angle = 0.0f;

			// compute the height of camera
			cameraH += cameraHOffset * timestep.GetSeconds();
			if (cameraH >= 10.0f)
				cameraHOffset = -5.0f;
			if (cameraH <= -10.0f)
				cameraHOffset = 5.0f;

			// draw the scene
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			for (int i = 0; i < OBJ_NUM; ++i)
			{
				dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &matrixs_[i]);
				objs_[i]->DrawSubset(0);
			}

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		for (auto& obj : objs_)
			SAFE_RELEASE(obj);
	}

	enum { OBJ_NUM = 5 };
private:
	ID3DXMesh* objs_[OBJ_NUM];
	D3DXMATRIX matrixs_[OBJ_NUM];
};

int main()
{
	D3DXCreate app;
	app.Create(640, 480, "D3DXCreate");
	app.Loop();
	return 0;
}