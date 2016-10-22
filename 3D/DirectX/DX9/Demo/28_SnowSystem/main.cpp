#include "sims.h"
#include "math/vector3.h"
#include "math/rng.h"
#include "core/input_state.h"
#include "utils/demo_app.h"
#include "particle_system.h"
#include "camera.h"
using namespace sims;

class SnowParticle : public ParticleSystem
{
public:
	SnowParticle(const BBox& bbox, uint32 numPartices)
		: rng_(20160926)
	{
		bbox_ = bbox;
		size_ = 0.25f;
		vbSize_ = 2048;
		vbOffset_ = 0;
		vbBatchSize_ = 512;

		for (uint32 i = 0; i < numPartices; ++i)
			AddParticle();
	}

	void ResetParticle(Attribute* attr)
	{
		attr->isAlive = true;

		// get random x, z, coordinate for the position of snow flake
		GetRandomVector(attr->position, bbox_.minP_, bbox_.maxP_);

		// no randomness for height(y-axis). snow flake always starts at top
		// of bounding box
		attr->position.y = bbox_.maxP_.y;

		// snow flakes fall downwards and slightly to the left;
		attr->velocity.x = rng_.RandomFloat(0.0f, 1.0f) * -3.0f;
		attr->velocity.y = rng_.RandomFloat(0.0f, 1.0f) * -10.0f;
		attr->velocity.z = 0.0f;

		// white snow flake
		attr->color = Color(255, 255, 255);
	}

	void Update(const Timestep& timestep)
	{
		float ts = timestep.GetSeconds();
		for (auto& p : particles_)
		{
			p.position += p.velocity * ts;
			// is point outside bounds
			if (!bbox_.Inside(p.position))
			{
				ResetParticle(&p);
			}
		}
	}

	void GetRandomVector(Vector3f& v, const Vector3f& mi, const Vector3f& ma)
	{
		v.x = rng_.RandomFloat(mi.x, ma.x);
		v.y = rng_.RandomFloat(mi.y, ma.y);
		v.z = rng_.RandomFloat(mi.z, ma.z);
	}

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
		d3d9::CHECK_HR = device->CreateVertexBuffer(6 * sizeof(d3d9::VertexUVN),
			0,
			d3d9::VertexUVN::FVF,
			D3DPOOL_MANAGED,
			&floor,
			0);
		d3d9::VertexUVN* v = nullptr;
		d3d9::CHECK_HR = floor->Lock(0, 0, (void**)&v, 0);
		v[0] = d3d9::VertexUVN(Vector3f(-20.0f, -2.5f, -20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f));
		v[1] = d3d9::VertexUVN(Vector3f(-20.0f, -2.5f, 20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.0f, 0.0f));
		v[2] = d3d9::VertexUVN(Vector3f(20.0f, -2.5f, 20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(1.0f, 0.0f));

		v[3] = d3d9::VertexUVN(Vector3f(-20.0f, -2.5f, -20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f));
		v[4] = d3d9::VertexUVN(Vector3f(20.0f, -2.5f, 20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(1.0f, 0.0f));
		v[5] = d3d9::VertexUVN(Vector3f(20.0f, -2.5f, -20.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector2f(1.0f, 1.0f));
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

class Snow : public DemoApp<d3d9::Window>
{
public:
	Snow()
		: ps_(nullptr)
	{}

	virtual void OnCreate()
	{
		// create snow particles system
		BBox bbox(Vector3f(-10.0f, -10.0f, -10.0f), Vector3f(10.0f, 10.0f, 10.0f));
		ps_ = new SnowParticle(bbox, 5000);
		ps_->Init(d3d9::g_pD3DD, "snowflake.dds");

		// create basic scene
		DrawBasicScene(d3d9::g_pD3DD, 1.0f);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI / 4.0f,
			(float)width_ / height_,
			1.0f,
			5000.0f);
		d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
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
	}

	virtual void OnRender(const Timestep&)
	{
		if (d3d9::g_pD3DD)
		{
			D3DXMATRIX view;
			camera_.GetViewMatrix(&view);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

			// draw
			d3d9::CHECK_HR = d3d9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
			d3d9::CHECK_HR = d3d9::g_pD3DD->BeginScene();

			D3DXMATRIX I;
			D3DXMatrixIdentity(&I);
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &I);

			DrawBasicScene(d3d9::g_pD3DD, 1.0f);

			// order important. render snow last
			d3d9::CHECK_HR = d3d9::g_pD3DD->SetTransform(D3DTS_WORLD, &I);
			ps_->Render();

			d3d9::CHECK_HR = d3d9::g_pD3DD->EndScene();
			d3d9::CHECK_HR = d3d9::g_pD3DD->Present(0, 0, 0, 0);
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
	Snow app;
	app.Create(640, 480, "Snow");
	app.Loop();
	return 0;
}