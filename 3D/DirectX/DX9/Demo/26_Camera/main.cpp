#include "sims.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/matrix33.h"
#include "math/matrix44.h"
#include "core/log.h"
#include "core/key_event.h"
#include "core/input_state.h"
#include "graphics_api/sims_sdk_d3d9.h"
#include "utils/demo_app.h"
using namespace sims;


class Camera
{
public:
	enum CameraType { LandObject, Aircraft };
	
	Camera(CameraType cameraType = Aircraft) 
		: cameraType_(cameraType)
	{
		eye_  = Vector3f(0.0f, 0.0f, 0.0f);
		look_ = Vector3f(0.0f, 0.0f, 1.0f);
		up_   = Vector3f(0.0f, 1.0f, 0.0f);
		right_= Vector3f(1.0f, 0.0f, 0.0f);
	}

	void SetCameraType(CameraType type) { cameraType_ = type; }

	void SetEye(const Vector3f& eye) { eye_ = eye; }

	const Vector3f& GetEye() const { return eye_; }
	const Vector3f& GetRight() const { return right_; }
	const Vector3f& GetUp() const { return up_; }
	const Vector3f& GetLook() const { return look_; }

	// rotate on right vector
	void Pitch(float radian)
	{
		Matrix33f r;
		right_.Normalize();
		MatrixRotationAxis(r, right_, radian);

		up_ *= r;
		look_ *= r;
	}

	// rotate on up vector
	void Yaw(float radian)
	{
		Matrix33f r;
		up_.Normalize();

		if (cameraType_ == LandObject) // rotate on y-aixs (0,1,0) for land object
			MatrixRotationY(r, radian);
		else
			MatrixRotationAxis(r, up_, radian);

		right_ *= r;
		look_ *= r;
	}

	// rotate on look vector
	void Roll(float radian)
	{
		if (cameraType_ == Aircraft)
		{
			Matrix33f r;
			look_.Normalize();
			MatrixRotationAxis(r, look_, radian);

			up_ *= r;
			right_ *= r;
		}
	}

	// move left/right
	void Strafe(float units)
	{
		// move only on x-z plane
		if (cameraType_ == LandObject)
			eye_ += Vector3f(right_.x, 0.0f, right_.z) * units;
		else
			eye_ += right_ * units;
	}

	// move up/down
	void Fly(float units)
	{
		if (cameraType_ == LandObject)
			eye_.y += units;
		else
			eye_ += up_ * units;
	}

	// move forward/backward
	void Walk(float units)
	{
		// move only on x-z plane
		if (cameraType_ == LandObject)
			eye_ += Vector3f(look_.x, 0.0f, look_.z) * units;
		else
			eye_ += look_ * units;
	}

	void GetViewMatrix(D3DXMATRIX* view)
	{
		Matrix44f translationTrans;
		MaxtrixTranslation(translationTrans, -eye_);

		look_.Normalize();
		up_ = CrossProduct(look_, right_);
		up_.Normalize();

		right_ = CrossProduct(up_, look_);
		right_.Normalize();

		Matrix44f rotateTrans;
		auto& m = rotateTrans.m;
		m[0][0] = right_.x;   m[0][1] = up_.x;   m[0][2] = look_.x;   m[0][3] = 0.0f;
		m[1][0] = right_.y;   m[1][1] = up_.y;   m[1][2] = look_.y;   m[1][3] = 0.0f;
		m[2][0] = right_.z;   m[2][1] = up_.z;   m[2][2] = look_.z;   m[2][3] = 0.0f;
		m[3][0] = 0.0f;       m[3][1] = 0.0f;    m[3][2] = 0.0f;      m[3][3] = 1.0f;

		Matrix44f v;
		MatrixMultiply(v, translationTrans, rotateTrans);
		memcpy(view, &v, sizeof(v));
	}
private:
	CameraType cameraType_;
	Vector3f eye_;

	Vector3f look_;
	Vector3f up_;
	Vector3f right_;
};

//////////////////////////////////////////////////////////////////////////
void DrawBasicScene(IDirect3DDevice9* device, float scale)
{
	static IDirect3DVertexBuffer9* floor = nullptr;
	static IDirect3DTexture9*      tex = nullptr;
	static ID3DXMesh*              pillar = nullptr;

	if (!device)
	{
		SAFE_RELEASE(floor);
		SAFE_RELEASE(tex);
		SAFE_RELEASE(pillar);
	}
	else if (!floor && !tex && !pillar)
	{
		d3d9::CHECK_HR = device->CreateVertexBuffer(6 * sizeof(d3d9::VertexUVN),
			0,
			d3d9::VertexUVN::FVF,
			D3DPOOL_MANAGED,
			&floor,
			0);
		d3d9::VertexUVN* v = nullptr;
		d3d9::CHECK_HR = floor->Lock(0, 0, (void**)&v, 0);
		v[0] = d3d9::VertexUVN(Vector3f(-20.0f, -2.5f, -20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f));
		v[1] = d3d9::VertexUVN(Vector3f(-20.0f, -2.5f,  20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.0f, 0.0f));
		v[2] = d3d9::VertexUVN(Vector3f( 20.0f, -2.5f,  20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(1.0f, 0.0f));

		v[3] = d3d9::VertexUVN(Vector3f(-20.0f, -2.5f, -20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f));
		v[4] = d3d9::VertexUVN(Vector3f( 20.0f, -2.5f,  20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(1.0f, 0.0f));
		v[5] = d3d9::VertexUVN(Vector3f( 20.0f, -2.5f, -20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(1.0f, 1.0f));
		d3d9::CHECK_HR = floor->Unlock();

		d3d9::CHECK_HR = D3DXCreateCylinder(device,
			0.5f,
			0.5f,
			5.0f,
			20,
			20,
			&pillar,
			0);
		d3d9::CHECK_HR = D3DXCreateTextureFromFile(device,
			"desert.bmp",
			&tex);
	}
	else
	{
		// pre-render setup
		d3d9::CHECK_HR = device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3d9::CHECK_HR = device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// setup directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = white;
		light.Specular = white * 0.6f;
		light.Ambient = white * 0.4f;
		light.Direction = D3DXVECTOR3(0.707f, -0.707f, 0.707f);

		d3d9::CHECK_HR = device->SetLight(0, &light);
		d3d9::CHECK_HR = device->LightEnable(0, true);
		d3d9::CHECK_HR = device->SetRenderState(D3DRS_LIGHTING, true);
		d3d9::CHECK_HR = device->SetRenderState(D3DRS_SPECULARENABLE, true);
		d3d9::CHECK_HR = device->SetRenderState(D3DRS_NORMALIZENORMALS, true);

		// render
		D3DXMATRIX T, R, P, S;
		D3DXMatrixScaling(&S, scale, scale, scale);
		D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);

		// draw floor
		D3DXMatrixIdentity(&T);
		T = T * S;
		d3d9::CHECK_HR = device->SetTransform(D3DTS_WORLD, &T);
		D3DMATERIAL9 whiteMtrl;
		GenMaterial(whiteMtrl, white, white, white, black, 2.0f);
		d3d9::CHECK_HR = device->SetMaterial(&whiteMtrl);
		d3d9::CHECK_HR = device->SetTexture(0, tex);
		d3d9::CHECK_HR = device->SetStreamSource(0, floor, 0, sizeof(d3d9::VertexUVN));
		d3d9::CHECK_HR = device->SetFVF(d3d9::VertexUVN::FVF);
		d3d9::CHECK_HR = device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// draw pillars
		D3DMATERIAL9 blueMtrl;
		GenMaterial(blueMtrl, blue, blue, blue, black, 2.0f);
		d3d9::CHECK_HR = device->SetMaterial(&blueMtrl);
		d3d9::CHECK_HR = device->SetTexture(0, 0);
		for (int i = 0; i < 5; ++i)
		{
			D3DXMatrixTranslation(&T, -5.0f, 0.0f, -15.0f + (i * 7.5f));
			P = R * T * S;
			d3d9::CHECK_HR = device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);

			D3DXMatrixTranslation(&T, 5.0f, 0.0f, -15.0f + (i * 7.5f));
			P = R * T * S;
			d3d9::CHECK_HR = device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

class CameraApp : public DemoApp<d3d9::Window>
{
public:
	CameraApp() : ts_(0), camera_(Camera::Aircraft) {}

	virtual void OnCreate()
	{
		DemoApp<d3d9::Window>::OnCreate();

		DrawBasicScene(d3d9::g_pD3DD, 0.0f);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.25f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (d3d9::g_pD3DD)
		{
			ts_ = timestep.GetSeconds();

			if (InputState::GetKeyState('W'))
				camera_.Walk(4.0f * ts_);
			else if (InputState::GetKeyState('S'))
				camera_.Walk(-4.0f * ts_);
			else if (InputState::GetKeyState('A'))
				camera_.Strafe(-4.0f * ts_);
			else if (InputState::GetKeyState('D'))
				camera_.Strafe(4.0f * ts_);
			else if (InputState::GetKeyState('R'))
				camera_.Fly(4.0f * ts_);
			else if (InputState::GetKeyState('F'))
				camera_.Fly(-4.0f * ts_);
			else if (InputState::GetKeyState(KEY_UP))
				camera_.Pitch(1.0f * ts_);
			else if (InputState::GetKeyState(KEY_DOWN))
				camera_.Pitch(-1.0f * ts_);
			else if (InputState::GetKeyState(KEY_LEFT))
				camera_.Yaw(-1.0f * ts_);
			else if (InputState::GetKeyState(KEY_RIGHT))
				camera_.Yaw(1.0f * ts_);
			else if (InputState::GetKeyState('N'))
				camera_.Roll(1.0f * ts_);
			else if (InputState::GetKeyState('M'))
				camera_.Roll(-1.0f * ts_);

			D3DXMATRIX view;
			camera_.GetViewMatrix(&view);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			DrawBasicScene(d3d9::g_pD3DD, 1.0f);

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		DrawBasicScene(nullptr, 0.0f);
	}

private:
	Camera camera_;
	float ts_;
};

int main()
{
	CameraApp app;
	app.Create(640, 480, "Camera");
	app.Loop();
	return 0;
}