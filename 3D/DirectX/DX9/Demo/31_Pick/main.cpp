#include "sims.h"
#include "math/sphere.h"
#include "math/ray.h"
#include "core/log.h"
#include "core/mouse_event.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx9.h"
using namespace sims;

class Pick : public DemoApp<dx9::Window>
{
public:
	Pick()
		: teapot_(nullptr)
		, sphere_(nullptr)
	{}

	virtual void OnCreate()
	{
		// create teapot
		dx9::CHECK_HR = D3DXCreateTeapot(dx9::g_pD3DD, &teapot_, 0);

		// compute bounding sphere
		uint8* v = nullptr;
		dx9::CHECK_HR = teapot_->LockVertexBuffer(0, (void**)&v);
		dx9::CHECK_HR = D3DXComputeBoundingSphere((D3DXVECTOR3*)v,
			teapot_->GetNumVertices(),
			D3DXGetFVFVertexSize(teapot_->GetFVF()),
			(D3DXVECTOR3*)&boundSphere.center_,
			&boundSphere.radius_);
		dx9::CHECK_HR = teapot_->UnlockVertexBuffer();

		// building a sphere mesh that describes the teapot's bounding sphere
		dx9::CHECK_HR = D3DXCreateSphere(dx9::g_pD3DD,
			boundSphere.radius_,
			20,
			20,
			&sphere_,
			0);

		// setup directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = white;
		light.Specular = white * 0.6f;
		light.Ambient = white * 0.4f;
		light.Direction = D3DXVECTOR3(0.707f, 0.0f, 0.707f);

		dx9::CHECK_HR = dx9::g_pD3DD->SetLight(0, &light);
		dx9::CHECK_HR = dx9::g_pD3DD->LightEnable(0, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SPECULARENABLE, false);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_NORMALIZENORMALS, true);

		// material
		GenMaterial(blueMtrl_, blue, blue, blue, black, 2.0f);
		blueMtrl_.Diffuse.a = 0.25f; // 25% opacity
		GenMaterial(yellowMtrl_, yellow, yellow, yellow, black, 2.0f);

		// view
		D3DXVECTOR3 pos(0.0f, 0.0f, -10.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &pos, &target, &up);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			// update teapot
			static float r = 0.0f;
			static float v = 1.0f;
			static float angle = 0.0f;

			D3DXMATRIX world;
			D3DXMatrixTranslation(&world, cosf(angle) * r, sinf(angle) * r, 10.0f);

			//transform the bounding sphere to match the teapots position in the world
			boundSphere.center_ = Vector3f(cosf(angle) * r, sinf(angle) * r, 10.0f);

			r += v * timestep.GetSeconds();
			if (r > 8.0f)
				v = -v;
			if (r <= 0.0f)
				v = -v;

			angle += 1.0f * D3DX_PI * timestep.GetSeconds();
			if (angle >= 2.0f * D3DX_PI)
				angle = 0.0f;

			// render
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			//// render teapot
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &world);
			dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&yellowMtrl_);
			dx9::CHECK_HR = teapot_->DrawSubset(0);

			// render bounding sphere with alpha blending
			dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&blueMtrl_);
			dx9::CHECK_HR = sphere_->DrawSubset(0);
			dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, false);


			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);

		}
	}

	virtual bool OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &Pick::OnMousePressEvent);
		return dispatch.GetResult();
	}

	bool OnMousePressEvent(const MousePressEvent& event)
	{
		if (event.GetButtonType() == MOUSE_LBUTTON)
		{
			D3DXMATRIX proj;
			dx9::CHECK_HR = dx9::g_pD3DD->GetTransform(D3DTS_PROJECTION, &proj);
			Matrix44f m;
			memcpy(&m, &proj, sizeof(proj));

			// compute the ray in view space
			Ray ray = GetViewRay(event.GetX(),
				event.GetY(),
				width_,
				height_,
				m);

			// transform ray to world
			D3DXMATRIX view;
			dx9::CHECK_HR = dx9::g_pD3DD->GetTransform(D3DTS_VIEW, &view);
			D3DXMATRIX viewINV;
			D3DXMatrixInverse(&viewINV, 0, &view);
			memcpy(&m, &viewINV, sizeof(viewINV));
			Ray rayt = TransformRay(ray, m);

			// test for hit
			float t = 0.0f;
			if (Intersect(rayt, boundSphere, t))
			{
				LOG_INFO("hit: %d, %d", event.GetX(), event.GetY());
			}
		}
		return true;
	}

	virtual void OnDestroy()
	{
		SAFE_RELEASE(teapot_);
		SAFE_RELEASE(sphere_);
	}
private:
	ID3DXMesh* teapot_;
	ID3DXMesh* sphere_;
	Sphere boundSphere;

	D3DMATERIAL9 blueMtrl_;
	D3DMATERIAL9 yellowMtrl_;
};

int main()
{
	Pick app;
	app.Create(640, 480, "Pick");
	app.Loop();
	return 0;
}