#include "sims.h"
#include "core/log.h"
#include "core/mouse_event.h"
#include "core/input_state.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_d3d11.h"
using namespace sims;

class BoxApp : public DemoApp<d3d11::Window>
{
public:
	BoxApp()
		: box_(nullptr)
		, vs_(nullptr)
		, ps_(nullptr)
		, mousePosX(0)
		, mousePosY(0)
		, radius_(5.0f)
		, theta_(1.5f * M_PI)
		, phi_(0.25f * M_PI)
	{}

	virtual void OnCreate()
	{
		// create vertex shader & pixel shader
		ID3D10Blob* errorMsg = nullptr;
		ID3D10Blob* vsBuffer = nullptr;
		ID3D10Blob* psBuffer = nullptr;
		d3d11::CHECK_HR = D3DX11CompileFromFile("box_vs.hlsl",
			0,
			0,
			"main",
			"vs_5_0",
#ifdef _DEBUG
			D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR,
#else
			D3D10_SHADER_PACK_MATRIX_ROW_MAJOR,
#endif
			0,
			0,
			&vsBuffer,
			&errorMsg,
			0);
		if (errorMsg)
		{
			LOG_ERROR("%s", (char*)errorMsg->GetBufferPointer());
			return;
		}

		d3d11::CHECK_HR = d3d11::g_pD3DD->CreateVertexShader(vsBuffer->GetBufferPointer(),
			vsBuffer->GetBufferSize(),
			0,
			&vs_);

		d3d11::CHECK_HR = D3DX11CompileFromFile("box_ps.hlsl",
			0,
			0,
			"main",
			"ps_5_0",
#ifdef _DEBUG
			D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR,
#else
			D3D10_SHADER_PACK_MATRIX_ROW_MAJOR,
#endif
			0,
			0,
			&psBuffer,
			&errorMsg,
			0);
		if (errorMsg)
		{
			LOG_ERROR("%s", (char*)errorMsg->GetBufferPointer());
			return;
		}

		d3d11::CHECK_HR = d3d11::g_pD3DD->CreatePixelShader(psBuffer->GetBufferPointer(),
			psBuffer->GetBufferSize(),
			0,
			&ps_);

		// matrix buffer in VS
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(Matrix44f);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		d3d11::CHECK_HR = d3d11::g_pD3DD->CreateBuffer(&desc, 0, &matrixBuffer_);

		// create box
		box_ = new d3d11::GeoBox<d3d11::Vertex>((uint8*)vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize());
		box_->Init(2, 2, 2, Matrix44f().Identity(), [](d3d11::Vertex* v, uint32 n)
		{
			Vector4f colors[] = {
				{ 1.0f, 1.0f, 1.0f, 1.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f },
				{ 1.0f, 0.0f, 0.0f, 1.0f },
				{ 0.0f, 1.0f, 0.0f, 1.0f },
				{ 0.0f, 0.0f, 1.0f, 1.0f },
				{ 1.0f, 1.0f, 0.0f, 1.0f },
				{ 0.0f, 1.0f, 1.0f, 1.0f },
				{ 1.0f, 0.0f, 1.0f, 1.0f }
			};
			for (uint32 i = 0; i < n; ++i)
			{
				v[i].c = colors[i];
			}
		});
	}

	virtual void OnUpdate(const Timestep& timestep)
	{
		Matrix44f worldM, viewM, projM;
		worldM.Identity();

		float x = radius_ * sinf(phi_) * cosf(theta_);
		float y = radius_ * cosf(phi_);
		float z = radius_ * sinf(phi_) * sinf(theta_);
		MatrixLookAtLH(viewM, Vector3f(x, y, z), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
		MatrixPerspectiveFovLH(projM, 0.25 * M_PI, (float)width_ / height_, 1.0f, 1000.0f);

		Matrix44f worldViewProjM = worldM * viewM * projM;

		// update WVP
		D3D11_MAPPED_SUBRESOURCE subRes;
		d3d11::CHECK_HR = d3d11::g_pD3DDC->Map(matrixBuffer_,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&subRes);
		Matrix44f* data = (Matrix44f*)subRes.pData;
		*data = worldViewProjM;
		d3d11::g_pD3DDC->Unmap(matrixBuffer_, 0);

		// update VS constant buffer
		d3d11::g_pD3DDC->VSSetConstantBuffers(0, 1, &matrixBuffer_);
	}

	virtual void OnRender(const Timestep&)
	{
		d3d11::g_pD3DDC->ClearRenderTargetView(d3d11::g_pRenderTargetView, (const float*)&d3d11::LightSteelBlue);
		d3d11::g_pD3DDC->ClearDepthStencilView(d3d11::g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		d3d11::g_pD3DDC->VSSetShader(vs_, 0, 0);
		d3d11::g_pD3DDC->PSSetShader(ps_, 0, 0);

		box_->Draw();

		d3d11::CHECK_HR = d3d11::g_pSwapChain->Present(0, 0);
	}

	virtual bool OnEvent(const Event& event)
	{
		EventDispatch dispatch(event);
		dispatch.Dispatch(this, &BoxApp::OnMouseMoveEvent);
		return dispatch.GetResult();
	}

	bool OnMouseMoveEvent(const MouseMoveEvent& event)
	{
		int x = event.GetX();
		int y = event.GetY();

		if (InputState::GetMouseState(MOUSE_LBUTTON))
		{
			// make each pixel correspond to a quarter of degree
			float dx = angle2radian((float)(x - mousePosX) * 0.25f);
			float dy = angle2radian((float)(y - mousePosY) * 0.25f);

			// update angles based on input to orbit camera around box
			theta_ += dx;
			phi_ += dy;

			// restrict the angle phi
			phi_ = clamp_t(phi_, 0.1f, M_PI - 0.1f);
		}
		else if (InputState::GetMouseState(MOUSE_RBUTTON))
		{
			// make each pixel correspond to 0.005 unit in the scene
			float dx = (float)(x - mousePosX) * 0.005f;
			float dy = (float)(y - mousePosY) * 0.005f;

			// update the camera radius based on input
			radius_ += (dx - dy);

			// restrict radius
			radius_ = clamp_t(radius_, 3.0f, 15.0f);
		}
		mousePosX = x;
		mousePosY = y;
		return true;
	}

	virtual void OnDestroy()
	{
		SAFE_DELETE(box_);
		SAFE_RELEASE(vs_);
		SAFE_RELEASE(ps_);
		SAFE_RELEASE(matrixBuffer_);
	}
private:
	d3d11::GeoBox<d3d11::Vertex>* box_;
	ID3D11VertexShader* vs_;
	ID3D11PixelShader*  ps_;
	ID3D11Buffer* matrixBuffer_;

	int mousePosX;
	int mousePosY;

	float radius_;
	float theta_;
	float phi_;
};

int main()
{
	BoxApp app;
	app.Create(800, 600, "Box");
	app.Loop();
	return 0;
}