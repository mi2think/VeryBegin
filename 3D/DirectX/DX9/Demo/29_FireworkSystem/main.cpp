#include "sims.h"
#include "math/vector3.h"
#include "math/rng.h"
#include "core/input_state.h"
#include "utils/demo_app.h"
#include "particle_system.h"
#include "camera.h"
using namespace sims;

class FireworkParticles : public ParticleSystem
{
public:
	FireworkParticles(const Vector3f& origin, uint32 numParticles)
		: rng_(20160926)
	{
		origin_ = origin;
		size_ = 0.9f;
		vbSize_ = 2048;
		vbOffset_ = 0;
		vbBatchSize_ = 512;

		for (uint32 i = 0; i < numParticles; ++i)
		{
			AddParticle();
		}
	}

	virtual void ResetParticle(Attribute* attr)
	{
		attr->isAlive = true;
		attr->position = origin_;

		Vector3f mi(-1.0f, -1.0f, -1.0f);
		Vector3f ma( 1.0f,  1.0f,  1.0f);

		GetRandomVector(attr->velocity, mi, ma);

		attr->velocity.Normalize();
		attr->velocity *= 100.0f;

		attr->color = Color(
			rng_.RandomUInt32(0, 255),
			rng_.RandomUInt32(0, 255),
			rng_.RandomUInt32(0, 255));
		attr->age = 0.0f;
		attr->lifeTime = 2.0f;
	}

	virtual void Update(const Timestep& timestep)
	{
		float ts = timestep.GetSeconds();
		for (auto& p : particles_)
		{
			if (p.isAlive)
			{
				p.position += p.velocity * ts;
				p.age += ts;
				if (p.age > p.lifeTime)
					p.isAlive = false;
			}
		}
	}

	virtual void PreRender()
	{
		ParticleSystem::PreRender();

		dx9::CHECK_HR = device_->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		dx9::CHECK_HR = device_->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		//read, but don't write to z buffer
		dx9::CHECK_HR = device_->SetRenderState(D3DRS_ZWRITEENABLE, false);
	}

	virtual void PostRender()
	{
		ParticleSystem::PostRender();

		dx9::CHECK_HR = device_->SetRenderState(D3DRS_ZWRITEENABLE, true);


	}

	void GetRandomVector(Vector3f& v, const Vector3f& mi, const Vector3f& ma)
	{
		v.x = rng_.RandomFloat(mi.x, ma.x);
		v.y = rng_.RandomFloat(mi.y, ma.y);
		v.z = rng_.RandomFloat(mi.z, ma.z);
	}
private:
	RNG rng_;
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
		dx9::CHECK_HR = device->CreateVertexBuffer(6 * sizeof(dx9::VertexUVN),
			0,
			dx9::VertexUVN::FVF,
			D3DPOOL_MANAGED,
			&floor,
			0);
		dx9::VertexUVN* v = nullptr;
		dx9::CHECK_HR = floor->Lock(0, 0, (void**)&v, 0);
		v[0] = dx9::VertexUVN(Vector3f(-20.0f, -2.5f, -20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f));
		v[1] = dx9::VertexUVN(Vector3f(-20.0f, -2.5f, 20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.0f, 0.0f));
		v[2] = dx9::VertexUVN(Vector3f(20.0f, -2.5f, 20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(1.0f, 0.0f));

		v[3] = dx9::VertexUVN(Vector3f(-20.0f, -2.5f, -20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f));
		v[4] = dx9::VertexUVN(Vector3f(20.0f, -2.5f, 20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(1.0f, 0.0f));
		v[5] = dx9::VertexUVN(Vector3f(20.0f, -2.5f, -20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(1.0f, 1.0f));
		dx9::CHECK_HR = floor->Unlock();

		dx9::CHECK_HR = D3DXCreateCylinder(device,
			0.5f,
			0.5f,
			5.0f,
			20,
			20,
			&pillar,
			0);
		dx9::CHECK_HR = D3DXCreateTextureFromFile(device,
			"desert.bmp",
			&tex);
	}
	else
	{
		// pre-render setup
		dx9::CHECK_HR = device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// setup directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = white;
		light.Specular = white * 0.6f;
		light.Ambient = white * 0.4f;
		light.Direction = D3DXVECTOR3(0.707f, -0.707f, 0.707f);

		dx9::CHECK_HR = device->SetLight(0, &light);
		dx9::CHECK_HR = device->LightEnable(0, true);
		dx9::CHECK_HR = device->SetRenderState(D3DRS_LIGHTING, true);
		dx9::CHECK_HR = device->SetRenderState(D3DRS_SPECULARENABLE, true);
		dx9::CHECK_HR = device->SetRenderState(D3DRS_NORMALIZENORMALS, true);

		// render
		D3DXMATRIX T, R, P, S;
		D3DXMatrixScaling(&S, scale, scale, scale);
		D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);

		// draw floor
		D3DXMatrixIdentity(&T);
		T = T * S;
		dx9::CHECK_HR = device->SetTransform(D3DTS_WORLD, &T);
		D3DMATERIAL9 whiteMtrl;
		GenMaterial(whiteMtrl, white, white, white, black, 2.0f);
		dx9::CHECK_HR = device->SetMaterial(&whiteMtrl);
		dx9::CHECK_HR = device->SetTexture(0, tex);
		dx9::CHECK_HR = device->SetStreamSource(0, floor, 0, sizeof(dx9::VertexUVN));
		dx9::CHECK_HR = device->SetFVF(dx9::VertexUVN::FVF);
		dx9::CHECK_HR = device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		// draw pillars
		D3DMATERIAL9 blueMtrl;
		GenMaterial(blueMtrl, blue, blue, blue, black, 2.0f);
		dx9::CHECK_HR = device->SetMaterial(&blueMtrl);
		dx9::CHECK_HR = device->SetTexture(0, 0);
		for (int i = 0; i < 5; ++i)
		{
			D3DXMatrixTranslation(&T, -5.0f, 0.0f, -15.0f + (i * 7.5f));
			P = R * T * S;
			dx9::CHECK_HR = device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);

			D3DXMatrixTranslation(&T, 5.0f, 0.0f, -15.0f + (i * 7.5f));
			P = R * T * S;
			dx9::CHECK_HR = device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

class Firework : public DemoApp<dx9::Window>
{
public:
	Firework()
		: ps_(nullptr)
	{}

	virtual void OnCreate()
	{
		// create firework particles system
		Vector3f origion(0.0f, 10.0f, 50.0f);
		ps_ = new FireworkParticles(origion, 6000);
		ps_->Init(dx9::g_pD3DD, "flare.bmp");

		// create basic scene
		DrawBasicScene(dx9::g_pD3DD, 1.0f);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI / 4.0f,
			(float)width_ / height_,
			1.0f,
			5000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnUpdate(const Timestep& timestep)
	{
		float ts_ = timestep.GetSeconds();

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

		ps_->Update(timestep);
		if (ps_->isDead())
			ps_->Reset();
	}

	virtual void OnRender(const Timestep&)
	{
		if (dx9::g_pD3DD)
		{
			D3DXMATRIX view;
			camera_.GetViewMatrix(&view);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

			// draw
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			D3DXMATRIX I;
			D3DXMatrixIdentity(&I);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &I);

			DrawBasicScene(dx9::g_pD3DD, 1.0f);

			// order important. render snow last
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &I);
			ps_->Render();

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual void OnDestroy()
	{
		SAFE_DELETE(ps_);
		DrawBasicScene(nullptr, 1.0f);
	}
private:
	ParticleSystem* ps_;
	Camera camera_;
};


int main()
{
	Firework app;
	app.Create(640, 480, "Firework");
	app.Loop();
	return 0;
}