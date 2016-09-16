#include "sims.h"
#include "core/key_event.h"
#include "math/bbox.h"
#include "graphics_api/sims_dx9.h"
#include "graphics_api/sims_dx9_geometry_def.h"
#include "utils/demo_app.h"
using namespace sims; 

void ComputeBoundingSphere(ID3DXMesh* mesh, BBox& bbox)
{
	uint8* v = nullptr;
	dx9::CHECK_HR = mesh->LockVertexBuffer(0, (void**)&v);

	D3DXVECTOR3 c;
	float r;
	dx9::CHECK_HR = D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		mesh->GetNumVertices(),
		D3DXGetFVFVertexSize(mesh->GetFVF()),
		&c,
		&r);
	bbox = BBox(Vector3f(c.x, c.y, c.z), r);

	dx9::CHECK_HR = mesh->UnlockVertexBuffer();
}

void ComputeBoundingBox(ID3DXMesh* mesh, BBox& bbox)
{
	uint8* v = nullptr;
	dx9::CHECK_HR = mesh->LockVertexBuffer(0, (void**)&v);

	D3DXVECTOR3 mi;
	D3DXVECTOR3 ma;
	dx9::CHECK_HR = D3DXComputeBoundingBox(
		(D3DXVECTOR3*)v,
		mesh->GetNumVertices(),
		D3DXGetFVFVertexSize(mesh->GetFVF()),
		&mi,
		&ma);
	bbox = BBox(Vector3f(mi.x, mi.y, mi.z), Vector3f(ma.x, ma.y, ma.z));

	dx9::CHECK_HR = mesh->UnlockVertexBuffer();
}

class BoundingVolumes : public DemoApp<dx9::Window>
{
public:
	BoundingVolumes()
		: mesh_(nullptr)
		, meshBox_(nullptr)
		, meshSphere_(nullptr)
		, drawBox_(false)
	{}

	virtual void OnCreate()
	{
		DemoApp<dx9::Window>::OnCreate();

		ID3DXBuffer* adjBuffer = nullptr;
		ID3DXBuffer* mtrlBuffer = nullptr;
		DWORD numMtrls = 0;

		// load x file
		dx9::CHECK_HR = D3DXLoadMeshFromX("bigship1.x",
			D3DXMESH_MANAGED,
			dx9::g_pD3DD,
			&adjBuffer,
			&mtrlBuffer,
			0,
			&numMtrls,
			&mesh_);

		// load materials
		if (mtrlBuffer != nullptr && numMtrls != 0)
		{
			D3DXMATERIAL* mts = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();
			for (int i = 0; i < numMtrls; ++i)
			{
				// MatD3D property does not have an ambient value when its loaded, so set it now
				auto& matD3D = mts[i].MatD3D;

				// the MatD3D property does not have an ambient value set when its loaded, so set it now
				matD3D.Ambient = matD3D.Diffuse;

				mts_.push_back(matD3D);

				IDirect3DTexture9* tex = nullptr;
				if (mts[i].pTextureFilename != nullptr)
				{
					dx9::CHECK_HR = D3DXCreateTextureFromFile(
						dx9::g_pD3DD,
						mts[i].pTextureFilename,
						&tex);
				}
				texs_.push_back(tex);
			}
		}
		SAFE_RELEASE(mtrlBuffer);

		// optimize mesh
		dx9::CHECK_HR = mesh_->OptimizeInplace(
			D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
			(DWORD*)adjBuffer->GetBufferPointer(),
			0,
			0,
			0);

		SAFE_RELEASE(adjBuffer);

		// create bounding sphere and bounding box
		BBox box, sphere;
		ComputeBoundingBox(mesh_, box);
		ComputeBoundingSphere(mesh_, sphere);

		dx9::CHECK_HR = D3DXCreateBox(dx9::g_pD3DD,
			box.Size().x,
			box.Size().y,
			box.Size().z,
			&meshBox_,
			0);
		Vector3f c;
		float r;
		sphere.InnerSphere(c, r);
		D3DXVECTOR3 xc(c.x, c.y, c.z);
		dx9::CHECK_HR = D3DXCreateSphere(dx9::g_pD3DD,
			r,
			20,
			20,
			&meshSphere_,
			0);

		// set texture filters
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		dx9::CHECK_HR = dx9::g_pD3DD->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// setup directional light
		D3DLIGHT9 light;
		memset(&light, 0, sizeof(light));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Diffuse = white;
		light.Specular = white * 0.6f;
		light.Ambient = white * 0.4f;
		light.Direction = D3DXVECTOR3(1.0f, -1.0f, 1.0f);

		dx9::CHECK_HR = dx9::g_pD3DD->SetLight(0, &light);
		dx9::CHECK_HR = dx9::g_pD3DD->LightEnable(0, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_LIGHTING, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SPECULARENABLE, true);
		dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_NORMALIZENORMALS, true);

		// view
		D3DXVECTOR3 pos(4.0f, 12.0f, -20.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX view;
		D3DXMatrixLookAtLH(&view, &pos, &target, &up);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_VIEW, &view);

		// projection
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(&proj,
			D3DX_PI * 0.5f,
			(float)width_ / height_,
			1.0f,
			1000.0f);
		dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_PROJECTION, &proj);
	}

	virtual void OnRender(const Timestep& timestep)
	{
		if (dx9::g_pD3DD)
		{
			static float y = 0.0f;

			D3DXMATRIX ry;
			D3DXMatrixRotationY(&ry, y);
			y += timestep.GetSeconds();
			if (y >= 2 * D3DX_PI)
				y = 0.0f;
			dx9::CHECK_HR = dx9::g_pD3DD->SetTransform(D3DTS_WORLD, &ry);

			// draw
			dx9::CHECK_HR = dx9::g_pD3DD->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->BeginScene();

			// mesh
			for (int i = 0; i < mts_.size(); ++i)
			{
				auto& mtrl = mts_[i];
				dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&mtrl);
				dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, texs_[i]);
				dx9::CHECK_HR = mesh_->DrawSubset(i);
			}

			// bounding volume in blue and at 10% opacity
			D3DMATERIAL9 mBlue;
			GenMaterial(mBlue, blue, blue, blue, black, 2.0f);
			mBlue.Diffuse.a = 0.10f;
			dx9::CHECK_HR = dx9::g_pD3DD->SetMaterial(&mBlue);
			dx9::CHECK_HR = dx9::g_pD3DD->SetTexture(0, 0);
			dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			if (drawBox_)
				meshBox_->DrawSubset(0);
			else
				meshSphere_->DrawSubset(0);
			dx9::CHECK_HR = dx9::g_pD3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

			dx9::CHECK_HR = dx9::g_pD3DD->EndScene();
			dx9::CHECK_HR = dx9::g_pD3DD->Present(0, 0, 0, 0);
		}
	}

	virtual bool OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &BoundingVolumes::OnKeyPressEvent);
		return dispatch.GetResult();
	}

	bool OnKeyPressEvent(const KeyPressEvent& event)
	{
		int key = event.GetKey();
		if (key == KEY_SPACE)
			drawBox_ = !drawBox_;
		return true;
	}


	virtual void OnDestroy()
	{
		SAFE_RELEASE(mesh_);
		SAFE_RELEASE(meshBox_);
		SAFE_RELEASE(meshSphere_);
		for (auto& tex : texs_)
		{
			SAFE_RELEASE(tex);
		}
	}
private:
	ID3DXMesh* mesh_;
	ID3DXMesh* meshBox_;
	ID3DXMesh* meshSphere_;
	bool drawBox_;
	vector<D3DMATERIAL9> mts_;
	vector<IDirect3DTexture9*> texs_;
};

int main()
{
	BoundingVolumes app;
	app.Create(640, 480, "Bounding Volumes");
	app.Loop();
	return 0;
}