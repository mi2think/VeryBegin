#include "sims.h"
#include "core/log.h"
#include "utils/demo_app.h"
#include "graphics_api/sims_sdk_dx11.h"
using namespace sims;

class BoxApp : public DemoApp<dx11::Window>
{
public:
	BoxApp()
		: box_(nullptr)
		, vs_(nullptr)
		, ps_(nullptr)
	{}

	virtual void OnCreate()
	{
		// create vertex shader & pixel shader
		ID3D10Blob* errorMsg = nullptr;
		ID3D10Blob* vsBuffer = nullptr;
		ID3D10Blob* psBuffer = nullptr;
		dx11::CHECK_HR = D3DX11CompileFromFile("box_vs.hlsl",
			0,
			0,
			"main",
			"vs_5_0",
#ifdef _DEBUG
			D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION,
#else
			0,
#endif
			0,
			0,
			&errorMsg,
			&vsBuffer,
			0);
		if (errorMsg)
		{
			LOG_ERROR("%s", (char*)errorMsg->GetBufferPointer());
			return;
		}

		dx11::CHECK_HR = dx11::g_pD3DD->CreateVertexShader(vsBuffer->GetBufferPointer(),
			vsBuffer->GetBufferSize(),
			0,
			&vs_);

		dx11::CHECK_HR = D3DX11CompileFromFile("box_ps.hlsl",
			0,
			0,
			"main",
			"ps_5_0",
#ifdef _DEBUG
			D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION,
#else
			0,
#endif
			0,
			0,
			&errorMsg,
			&psBuffer,
			0);
		if (errorMsg)
		{
			LOG_ERROR("%s", (char*)errorMsg->GetBufferPointer());
			return;
		}

		dx11::CHECK_HR = dx11::g_pD3DD->CreatePixelShader(psBuffer->GetBufferPointer(),
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
		dx11::CHECK_HR = dx11::g_pD3DD->CreateBuffer(&desc, 0, &matrixBuffer_);

		// create box
		box_ = new dx11::GeoBox<dx11::Vertex>((uint8*)vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize());
		box_->Init(2, 2, 2, Matrix44f().Identity(), [](dx11::Vertex* v, uint32 n)
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
	}

	virtual void OnRender(const Timestep&)
	{
		dx11::g_pD3DDC->ClearRenderTargetView(dx11::g_pRenderTargetView, (const float*)&dx11::LightSteelBlue);
		dx11::g_pD3DDC->ClearDepthStencilView(dx11::g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		dx11::CHECK_HR = dx11::g_pSwapChain->Present(0, 0);
	}

private:
	dx11::GeoBox<dx11::Vertex>* box_;
	ID3D11VertexShader* vs_;
	ID3D11PixelShader*  ps_;
	ID3D11Buffer* matrixBuffer_;
};

int main()
{
	BoxApp app;
	app.Create(800, 600, "Box");
	app.Loop();
	return 0;
}