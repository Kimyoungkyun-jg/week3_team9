#include "FRenderer.h"

#include "ShaderConstants.h"
#include "FMesh.h"
#include "FMaterial.h"
#include "FRenderPipeline.h"
#include "Vertices.h"
#include "Runtime/Core/PointerTypes.h"
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

bool FRenderer::Initialize(HWND Window)
{
	if (!InitializeDeviceAndSwapChain(Window) ||
		!InitializeBackBufferAndDepthStencil() ||
		!InitializeConstantBuffers()||
		!InitializeGridConstantBuffers())
	{
		Shutdown();
		return false;
	}

	return true;
}

void FRenderer::Shutdown()
{
	if (Context)
	{
		Context->ClearState();
		Context->Flush();
	}

	ObjectConstantBuffer.Reset();
	GridConstantBuffer.Reset();

	BackBufferRTV.Reset();
	DepthStencilView.Reset();
	DepthStencilBuffer.Reset();

	SwapChain.Reset();
	Context.Reset();
	Device.Reset();
}

void FRenderer::BeginFrame()
{
	Context->RSSetViewports(1, &Viewport);
	Context->OMSetRenderTargets(1, BackBufferRTV.GetAddressOf(), DepthStencilView.Get());


	constexpr float ClearColor[] = { 0.05f, 0.05f, 0.08f, 1.0f };
	Context->ClearRenderTargetView(BackBufferRTV.Get(), ClearColor);
	Context->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void FRenderer::SetViewportUV(FVector2 TopLeftUV, FVector2 LengthUV)
{
	// Viewport는 전체 백버퍼 크기를 유지하고, UV는 그리기 직전에 픽셀로 변환한다.
	D3D11_VIEWPORT RenderViewport = Viewport;
	RenderViewport.TopLeftX = TopLeftUV.X * Viewport.Width;
	RenderViewport.TopLeftY = TopLeftUV.Y * Viewport.Height;
	RenderViewport.Width = LengthUV.X * Viewport.Width;
	RenderViewport.Height = LengthUV.Y * Viewport.Height;
	Context->RSSetViewports(1, &RenderViewport);

	FFrameConstants Constants{ FVector2{ RenderViewport.Width, RenderViewport.Height } };
	Context->UpdateSubresource(FrameConstantBuffer.Get(), 0, nullptr, &Constants, 0, 0);
	Context->VSSetConstantBuffers(1, 1, FrameConstantBuffer.GetAddressOf());
	Context->PSSetConstantBuffers(1, 1, FrameConstantBuffer.GetAddressOf());
};

void FRenderer::Draw(const FMesh& Mesh, const FMaterial& Material, const FObjectConstants& ObjectConstants)
{
	UpdateObjectConstants(ObjectConstants);

	if (Mesh.GetVertexLayout() != Material.GetVertexLayout())
	{
		return;
	}

	const auto& Pipeline = Material.Pipeline;

	Pipeline->Bind(*Context.Get());
	Material.BindResources(*Context.Get());
	Mesh.BindResources(*Context.Get());

	Context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	if (Mesh.HasIndices())
	{
		Context->DrawIndexed(Mesh.IndexCount, 0, 0);
	}
	else
	{
		Context->Draw(Mesh.VertexCount, 0);
	}
}

void FRenderer::DrawGrid(const FMesh& Mesh, const FMaterial& Material, const FGridConstants& GridConstants)
{
	UpdateGridConstants(GridConstants);

	if (Mesh.GetVertexLayout() != Material.GetVertexLayout())
	{
		return;
	}

	const auto& Pipeline = Material.Pipeline;
		
	Pipeline->Bind(*Context.Get());
	Material.BindResources(*Context.Get());
	Mesh.BindResources(*Context.Get());

	Context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	if (Mesh.HasIndices())
	{
		Context->DrawIndexed(Mesh.IndexCount, 0, 0);
	}
	else
	{
		Context->Draw(Mesh.VertexCount, 0);
	}
}

void FRenderer::ClearDepth()
{
	Context->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


void FRenderer::SwapBuffer()
{
	SwapChain->Present(1u, 0u);
}

void FRenderer::OnWindowSize(UINT Width, UINT Height)
{
	Context->OMSetRenderTargets(0, nullptr, nullptr);
	BackBufferRTV.Reset();
	DepthStencilView.Reset();
	DepthStencilBuffer.Reset();

	SwapChain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, 0);
	Viewport.Width = static_cast<float>(Width);
	Viewport.Height = static_cast<float>(Height);

	InitializeBackBufferAndDepthStencil();
}

TSharedPtr<FMesh> FRenderer::CreateMesh(const FMeshDesc& Desc)
{
	if (!Desc.VertexData ||
		Desc.VertexCount == 0 ||
		Desc.VertexDataSize == 0 ||
		Desc.VertexStride == 0)
	{
		return nullptr;
	}
	if (Desc.IndexCount > 0 && (!Desc.IndexData || Desc.IndexDataSize == 0))
	{
		return nullptr;
	}

	auto Mesh = TSharedPtr<FMesh>{ new FMesh() };
	Mesh->VertexLayout = Desc.VertexLayout;

	D3D11_BUFFER_DESC VertexBufferDesc = {
		.ByteWidth = Desc.VertexDataSize,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
	};

	D3D11_SUBRESOURCE_DATA VertexData = {
		.pSysMem = Desc.VertexData,
	};

	HRESULT Result = Device->CreateBuffer(&VertexBufferDesc, &VertexData, &Mesh->VertexBuffer);
	if (FAILED(Result))
	{
		return nullptr;
	}
	Mesh->VertexCount = Desc.VertexCount;
	Mesh->VertexStride = Desc.VertexStride;

	if (Desc.IndexCount > 0 && Desc.IndexData)
	{
		D3D11_BUFFER_DESC IndexBufferDesc = {
			.ByteWidth = Desc.IndexDataSize,
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_INDEX_BUFFER,
		};

		D3D11_SUBRESOURCE_DATA IndexData = {
			.pSysMem = Desc.IndexData,
		};

		Result = Device->CreateBuffer(&IndexBufferDesc, &IndexData, &Mesh->IndexBuffer);
		if (FAILED(Result))
		{
			return nullptr;
		}
	}
	Mesh->IndexCount = Desc.IndexCount;

	const auto* vertices =
		static_cast<const FVertexPositionColor*>(Desc.VertexData);

	Mesh->Positions.reserve(Desc.VertexCount);
	for (uint32 i = 0; i < Desc.VertexCount; ++i)
	{
		Mesh->Positions.push_back(vertices[i].Position);
	}

	if (Desc.IndexCount > 0)
	{
		const auto* indices = static_cast<const uint32*>(Desc.IndexData);
		Mesh->Indices.assign(indices, indices + Desc.IndexCount);
	}

	Mesh->Topology = Desc.bIsLine ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	return Mesh;
}

TSharedPtr<FMaterial> FRenderer::CreateMaterial(const FMaterialDesc& Desc)
{
	TSharedPtr<FMaterial> Material{ new FMaterial() };

	Material->Pipeline = FindOrCreateRenderPipeline(Desc);
	if (!Material->Pipeline)
	{
		return nullptr;
	}

	return Material;
}

void FRenderer::GetDeviceAndContext_ImplDX11(ID3D11Device*& DeviceOut, ID3D11DeviceContext*& ContextOut) {
	DeviceOut = Device.Get();
	ContextOut = Context.Get();
}

bool FRenderer::InitializeDeviceAndSwapChain(HWND Window)
{
	constexpr D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC SwapChainDesc{
		.BufferDesc = {
			.Width = 0u,
			.Height = 0u,
			.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
		},
		.SampleDesc = {
			.Count = 1u,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2u,
		.OutputWindow = Window,
		.Windowed = true,
		.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
	};

	UINT CreateDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifndef NDEBUG
	CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT Result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		CreateDeviceFlags,
		FeatureLevels,
		ARRAYSIZE(FeatureLevels),
		D3D11_SDK_VERSION,
		&SwapChainDesc,
		&SwapChain,
		&Device,
		nullptr,
		&Context);
	if (FAILED(Result))
	{
		return false;
	}

	RECT ClientRect{};
	GetClientRect(Window, &ClientRect);

	Viewport = {
		.TopLeftX = 0.0f,
		.TopLeftY = 0.0f,
		.Width = static_cast<float>(ClientRect.right - ClientRect.left),
		.Height = static_cast<float>(ClientRect.bottom - ClientRect.top),
		.MinDepth = 0.0f,
		.MaxDepth = 1.0f,
	};

	return true;
}

bool FRenderer::InitializeBackBufferAndDepthStencil()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> BackBuffer;
	HRESULT Result = SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer));
	if (FAILED(Result))
	{
		return false;
	}

	Result = Device->CreateRenderTargetView(BackBuffer.Get(), nullptr, &BackBufferRTV);
	if (FAILED(Result))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC DepthStencilDesc = {
		.Width = static_cast<UINT>(Viewport.Width),
		.Height = static_cast<UINT>(Viewport.Height),
		.MipLevels = 1u,
		.ArraySize = 1u,
		.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
		.SampleDesc = {
			.Count = 1u,
		},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_DEPTH_STENCIL,
	};

	Result = Device->CreateTexture2D(&DepthStencilDesc, nullptr, &DepthStencilBuffer);
	if (FAILED(Result))
	{
		return false;
	}

	Result = Device->CreateDepthStencilView(DepthStencilBuffer.Get(), nullptr, &DepthStencilView);
	if (FAILED(Result))
	{
		return false;
	}

	return true;
}

bool FRenderer::InitializeConstantBuffers()
{
	D3D11_BUFFER_DESC ObjectConstantBufferDesc = {
		.ByteWidth = sizeof(FObjectConstants),
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
	};

	HRESULT Result = Device->CreateBuffer(&ObjectConstantBufferDesc, nullptr, &ObjectConstantBuffer);
	if (FAILED(Result))
	{
		return false;
	}

	D3D11_BUFFER_DESC FrameConstantBufferDesc = {
		.ByteWidth = sizeof(FFrameConstants),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
	};

	Result = Device->CreateBuffer(&FrameConstantBufferDesc, nullptr, &FrameConstantBuffer);
	if (FAILED(Result))
	{
		return false;
	}

	return true;
}

TSharedPtr<FRenderPipeline> FRenderer::FindOrCreateRenderPipeline(const FMaterialDesc& Desc)
{
	// TODO: 이미 만들었던 파이프라인 있는지 찾아서 쓰기

	TSharedPtr<FRenderPipeline> Pipeline{ new FRenderPipeline() };
	Pipeline->VertexLayout = Desc.VertexLayout;

	Microsoft::WRL::ComPtr<ID3DBlob> Blob;	
	HRESULT Result = D3DReadFileToBlob(Desc.VertexShaderFileName.c_str(), &Blob);
	if (FAILED(Result))
	{
		return nullptr;
	}

	Result = Device->CreateVertexShader(
		Blob->GetBufferPointer(),
		Blob->GetBufferSize(),
		nullptr,
		&Pipeline->VertexShader);
	if (FAILED(Result))
	{
		return nullptr;
	}

	FVertexLayoutDesc LayoutDesc = GetVertexLayoutDesc(Desc.VertexLayout);
	Result = Device->CreateInputLayout(LayoutDesc.InputElements, LayoutDesc.InputElementCount, Blob->GetBufferPointer(), Blob->GetBufferSize(), &Pipeline->InputLayout);
	if (FAILED(Result))
	{
		return nullptr;
	}

	Result = D3DReadFileToBlob(Desc.PixelShaderFileName.c_str(), &Blob);
	if (FAILED(Result))
	{
		return nullptr;
	}

	Result = Device->CreatePixelShader(
		Blob->GetBufferPointer(),
		Blob->GetBufferSize(),
		nullptr,
		&Pipeline->PixelShader);
	if (FAILED(Result))
	{
		return nullptr;
	}

	D3D11_RASTERIZER_DESC RasterizerDesc{
		.FillMode = D3D11_FILL_SOLID,
		.CullMode = D3D11_CULL_BACK,
		.FrontCounterClockwise = false,
	};

	Result = Device->CreateRasterizerState(&RasterizerDesc, &Pipeline->RasterizerState);
	if (FAILED(Result))
	{
		return nullptr;
	}

	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc{
		.DepthEnable = Desc.bEnableDepthTest,
		.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		.DepthFunc = D3D11_COMPARISON_LESS,
	};

	Result = Device->CreateDepthStencilState(&DepthStencilDesc, &Pipeline->DepthStencilState);
	if (FAILED(Result))
	{
		return nullptr;
	}

	return Pipeline;
}

bool FRenderer::InitializeGridConstantBuffers()
{
	D3D11_BUFFER_DESC GridConstantBufferDesc = {
		.ByteWidth = sizeof(FGridConstants),
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
	};

	HRESULT Result = Device->CreateBuffer(&GridConstantBufferDesc, nullptr, &GridConstantBuffer);
	if (FAILED(Result))
	{
		return false;
	}

	return true;
}

void FRenderer::UpdateObjectConstants(const FObjectConstants& Constants)
{
	static const FMatrix UnrealClipToD3DClip{
		FVector{ 0.0f, 0.0f, 1.0f },
		FVector{ 1.0f, 0.0f, 0.0f },
		FVector{ 0.0f, 1.0f, 0.0f },
		FVector{ 0.0f, 0.0f, 0.0f }
	};

	// 언리얼 Clip -> D3D Clip 좌표 변환
	FObjectConstants ShaderConstants = Constants;
	ShaderConstants.MVP *= UnrealClipToD3DClip;

	D3D11_MAPPED_SUBRESOURCE MappedResource{};
	Context->Map(ObjectConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	memcpy(MappedResource.pData, &ShaderConstants, sizeof(ShaderConstants));
	Context->Unmap(ObjectConstantBuffer.Get(), 0);

	Context->VSSetConstantBuffers(0, 1, ObjectConstantBuffer.GetAddressOf());
	Context->PSSetConstantBuffers(0, 1, ObjectConstantBuffer.GetAddressOf());
}

void FRenderer::UpdateGridConstants(const FGridConstants& Constants)
{
	static const FMatrix UnrealClipToD3DClip{
		FVector{ 0.0f, 0.0f, 1.0f },
		FVector{ 1.0f, 0.0f, 0.0f },
		FVector{ 0.0f, 1.0f, 0.0f },
		FVector{ 0.0f, 0.0f, 0.0f }
	};

	// 언리얼 Clip -> D3D Clip 좌표 변환
	FGridConstants ShaderConstants = Constants;
	ShaderConstants.MVP *= UnrealClipToD3DClip;

	D3D11_MAPPED_SUBRESOURCE MappedResource{};
	Context->Map(GridConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	memcpy(MappedResource.pData, &ShaderConstants, sizeof(ShaderConstants));
	Context->Unmap(GridConstantBuffer.Get(), 0);

	Context->VSSetConstantBuffers(0, 1, GridConstantBuffer.GetAddressOf());
	Context->PSSetConstantBuffers(0, 1, GridConstantBuffer.GetAddressOf());
}

