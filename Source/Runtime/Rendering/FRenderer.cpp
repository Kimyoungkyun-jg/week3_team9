#include "FRenderer.h"
#include "FRenderResourceLibrary.h"

#include "FMaterial.h"
#include "FMesh.h"
#include "FRenderPipeline.h"
#include "Runtime/Core/PointerTypes.h"
#include "Runtime/Rendering/FTexture.h"
#include "Runtime/Engine/FCamera.h"
#include "ShaderConstants.h"
#include "Vertices.h"
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>


bool FRenderer::Initialize(HWND Window) {
  if (!InitializeDeviceAndSwapChain(Window) ||
      !InitializeBackBufferAndDepthStencil() || !InitializeConstantBuffers()) {
    Shutdown();
    return false;
  }

  LineBatcher.Initialize(Device.Get()); // batch line

  return true;
}

void FRenderer::Shutdown() {
  if (Context) {
    Context->ClearState();
    Context->Flush();
  }

  LineBatcher.Shutdown();

  b0ConstantBuffer.Reset();
  FrameConstantBuffer.Reset();

  BackBufferRTV.Reset();
  DepthStencilView.Reset();
  DepthStencilBuffer.Reset();

  SwapChain.Reset();
  Context.Reset();
  Device.Reset();
}

void FRenderer::BeginFrame() {
  Context->RSSetViewports(1, &Viewport);
  Context->OMSetRenderTargets(1, BackBufferRTV.GetAddressOf(),
                              DepthStencilView.Get());

  constexpr float ClearColor[] = {0.05f, 0.05f, 0.08f, 1.0f};
  Context->ClearRenderTargetView(BackBufferRTV.Get(), ClearColor);
  Context->ClearDepthStencilView(
      DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void FRenderer::SetViewportUV(FVector2 TopLeftUV, FVector2 LengthUV) {
  // Viewport는 전체 백버퍼 크기를 유지하고, UV는 그리기 직전에 픽셀로 변환한다.
  D3D11_VIEWPORT RenderViewport = Viewport;
  RenderViewport.TopLeftX = TopLeftUV.X * Viewport.Width;
  RenderViewport.TopLeftY = TopLeftUV.Y * Viewport.Height;
  RenderViewport.Width = LengthUV.X * Viewport.Width;
  RenderViewport.Height = LengthUV.Y * Viewport.Height;
  Context->RSSetViewports(1, &RenderViewport);

  FFrameConstants Constants{
      FVector2{RenderViewport.Width, RenderViewport.Height}};
  Context->UpdateSubresource(FrameConstantBuffer.Get(), 0, nullptr, &Constants,
                             0, 0);
  Context->VSSetConstantBuffers(1, 1, FrameConstantBuffer.GetAddressOf());
  Context->PSSetConstantBuffers(1, 1, FrameConstantBuffer.GetAddressOf());
};

//void FRenderer::Draw(const FMesh &Mesh, const FMaterial &Material,
//                     const FObjectConstants &ObjectConstants) {
//  UpdateObjectConstants(ObjectConstants);
//
//  TSharedPtr<FRenderPipeline> Pipeline = Material.Pipeline;
//  if (CurrentRenderMode == EViewModeIndex::VMI_Wireframe) {
//    Pipeline = GetPipeline(EBuiltinPipeline::Simple_Wireframe);
//  }
//
//  if (Pipeline) {
//    Pipeline->Bind(*Context.Get());
//  }
//
//  Material.BindResources(*Context.Get());
//  Mesh.BindResources(*Context.Get());
//
//  Context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
//
//  if (Mesh.HasIndices()) {
//    Context->DrawIndexed(Mesh.IndexCount, 0, 0);
//  } else {
//    Context->Draw(Mesh.VertexCount, 0);
//  }
//}
//
//void FRenderer::DrawGrid(const FMesh &Mesh, const FMaterial &Material,
//                         const FGridConstants &GridConstants) {
//  UpdateGridConstants(GridConstants);
//  const auto &Pipeline = Material.Pipeline;
//
//  Pipeline->Bind(*Context.Get());
//  Material.BindResources(*Context.Get());
//  Mesh.BindResources(*Context.Get());
//
//  Context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
//
//  if (Mesh.HasIndices()) {
//    Context->DrawIndexed(Mesh.IndexCount, 0, 0);
//  } else {
//    Context->Draw(Mesh.VertexCount, 0);
//  }
//}

void FRenderer::ClearDepth() {
  Context->ClearDepthStencilView(
      DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void FRenderer::SwapBuffer() { SwapChain->Present(1u, 0u); }

void FRenderer::OnWindowSize(UINT Width, UINT Height) {
  Context->OMSetRenderTargets(0, nullptr, nullptr);
  BackBufferRTV.Reset();
  DepthStencilView.Reset();
  DepthStencilBuffer.Reset();

  SwapChain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, 0);
  Viewport.Width = static_cast<float>(Width);
  Viewport.Height = static_cast<float>(Height);

  InitializeBackBufferAndDepthStencil();
}

void FRenderer::FlushLineBatch(const FMatrix &ViewProjection) {
  LineBatcher.Flush(*Context.Get(), *this, ViewProjection);
}

TSharedPtr<FMesh> FRenderer::CreateMesh(const FMeshDesc &Desc) {
  if (!Desc.VertexData || Desc.VertexCount == 0 || Desc.VertexDataSize == 0 ||
      Desc.VertexStride == 0) {
    return nullptr;
  }
  if (Desc.IndexCount > 0 && (!Desc.IndexData || Desc.IndexDataSize == 0)) {
    return nullptr;
  }

  auto Mesh = TSharedPtr<FMesh>{new FMesh()};
  D3D11_BUFFER_DESC VertexBufferDesc = {
      .ByteWidth = Desc.VertexDataSize,
      .Usage = D3D11_USAGE_DEFAULT,
      .BindFlags = D3D11_BIND_VERTEX_BUFFER,
  };

  D3D11_SUBRESOURCE_DATA VertexData = {
      .pSysMem = Desc.VertexData,
  };

  HRESULT Result =
      Device->CreateBuffer(&VertexBufferDesc, &VertexData, &Mesh->VertexBuffer);
  if (FAILED(Result)) {
    return nullptr;
  }
  Mesh->VertexCount = Desc.VertexCount;
  Mesh->VertexStride = Desc.VertexStride;

  if (Desc.IndexCount > 0 && Desc.IndexData) {
    D3D11_BUFFER_DESC IndexBufferDesc = {
        .ByteWidth = Desc.IndexDataSize,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_INDEX_BUFFER,
    };

    D3D11_SUBRESOURCE_DATA IndexData = {
        .pSysMem = Desc.IndexData,
    };

    Result =
        Device->CreateBuffer(&IndexBufferDesc, &IndexData, &Mesh->IndexBuffer);
    if (FAILED(Result)) {
      return nullptr;
    }
  }
  Mesh->IndexCount = Desc.IndexCount;

  const auto *vertices = static_cast<const FVertexData *>(Desc.VertexData);

  Mesh->Positions.reserve(Desc.VertexCount);
  for (uint32 i = 0; i < Desc.VertexCount; ++i) {
    Mesh->Positions.push_back(
        FVector{vertices[i].x, vertices[i].y, vertices[i].z});
  }

  if (Desc.IndexCount > 0) {
    const auto *indices = static_cast<const uint32 *>(Desc.IndexData);
    Mesh->Indices.assign(indices, indices + Desc.IndexCount);
  }

  Mesh->Topology = Desc.bIsLine ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST
                                : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

  Mesh->LocalBounds = FAxisAlignedBoundingBox{*Mesh.get()};
  return Mesh;
}

TSharedPtr<FMesh> FRenderer::CreateDynamicMesh(const FMeshDesc& Desc) { 
    if (!Desc.VertexData || Desc.VertexCount == 0 || Desc.VertexDataSize == 0 ||
        Desc.VertexStride == 0) {
        return nullptr;
    }
    if (Desc.IndexCount > 0 && (!Desc.IndexData || Desc.IndexDataSize == 0)) {
        return nullptr;
    }

    auto Mesh = TSharedPtr<FMesh>{ new FMesh() };
    D3D11_BUFFER_DESC VertexBufferDesc = {
        .ByteWidth = Desc.VertexDataSize,
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };

    D3D11_SUBRESOURCE_DATA VertexData = {
        .pSysMem = Desc.VertexData,
    };

    HRESULT Result =
        Device->CreateBuffer(&VertexBufferDesc, &VertexData, &Mesh->VertexBuffer);
    if (FAILED(Result)) {
        return nullptr;
    }
    Mesh->VertexCount = Desc.VertexCount;
    Mesh->VertexStride = Desc.VertexStride;
    Mesh->VertexBufferSize = Desc.VertexDataSize;

    if (Desc.IndexCount > 0 && Desc.IndexData) {
        D3D11_BUFFER_DESC IndexBufferDesc = {
            .ByteWidth = Desc.IndexDataSize,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_INDEX_BUFFER,
        };

        D3D11_SUBRESOURCE_DATA IndexData = {
            .pSysMem = Desc.IndexData,
        };

        Result =
            Device->CreateBuffer(&IndexBufferDesc, &IndexData, &Mesh->IndexBuffer);
        if (FAILED(Result)) {
            return nullptr;
        }
    }
    Mesh->IndexCount = Desc.IndexCount;
    Mesh->IndexBufferSize = Desc.IndexDataSize;

    const auto* vertices = static_cast<const FVertexData*>(Desc.VertexData);

    Mesh->Positions.reserve(Desc.VertexCount);
    for (uint32 i = 0; i < Desc.VertexCount; ++i) {
        Mesh->Positions.push_back(
            FVector{ vertices[i].x, vertices[i].y, vertices[i].z });
    }

    if (Desc.IndexCount > 0) {
        const auto* indices = static_cast<const uint32*>(Desc.IndexData);
        Mesh->Indices.assign(indices, indices + Desc.IndexCount);
    }

    Mesh->Topology = Desc.bIsLine ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST
        : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    Mesh->LocalBounds = FAxisAlignedBoundingBox{ *Mesh.get() };
    return Mesh;
}

TSharedPtr<FMaterial> FRenderer::CreateMaterial(const FMaterialDesc &Desc) {
  TSharedPtr<FMaterial> Material{new FMaterial()};
  return Material;
}

void FRenderer::GetDeviceAndContext_ImplDX11(ID3D11Device *&DeviceOut,
                                             ID3D11DeviceContext *&ContextOut) {
  DeviceOut = Device.Get();
  ContextOut = Context.Get();
}

TSharedPtr<FRenderPipeline>
FRenderer::CreateRenderPipeline(const FRenderPipelineDesc &Desc,
                                EViewModeIndex RenderMode) {
  TSharedPtr<FRenderPipeline> Pipeline{new FRenderPipeline()};
  Pipeline->desc = Desc;

  Microsoft::WRL::ComPtr<ID3DBlob> Blob;
  HRESULT Result = D3DReadFileToBlob(Desc.VertexShaderFileName.c_str(), &Blob);
  if (FAILED(Result)) {
    return nullptr;
  }

  Result = Device->CreateVertexShader(Blob->GetBufferPointer(),
                                      Blob->GetBufferSize(), nullptr,
                                      &Pipeline->VertexShader);
  if (FAILED(Result)) {
    return nullptr;
  }

  if (Desc.bIsInstancing)
  {
      Result = Device->CreateInputLayout(FVertexInstanceLayouts::Layout, FVertexInstanceLayouts::NumElements,
          Blob->GetBufferPointer(), Blob->GetBufferSize(), &Pipeline->InputLayout);
  }
  else
  {
      Result = Device->CreateInputLayout(FVertexLayouts::Layout, FVertexLayouts::NumElements,
          Blob->GetBufferPointer(), Blob->GetBufferSize(), &Pipeline->InputLayout);
  }


  if (FAILED(Result)) {
      return nullptr;
  }

  Result = D3DReadFileToBlob(Desc.PixelShaderFileName.c_str(), &Blob);
  if (FAILED(Result)) {
    return nullptr;
  }

  Result =
      Device->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(),
                                nullptr, &Pipeline->PixelShader);
  if (FAILED(Result)) {
    return nullptr;
  }

  D3D11_RASTERIZER_DESC RasterizerDesc{
      .FillMode = (RenderMode == EViewModeIndex::VMI_Wireframe)
                      ? D3D11_FILL_WIREFRAME
                      : D3D11_FILL_SOLID,
      .CullMode = Desc.CullMode,
      .FrontCounterClockwise = false,
  };

  Result = Device->CreateRasterizerState(&RasterizerDesc,
                                         &Pipeline->RasterizerState);
  if (FAILED(Result)) {
    return nullptr;
  }

  D3D11_DEPTH_STENCIL_DESC DepthStencilDesc{
      .DepthEnable = Desc.bEnableDepthTest,
      .DepthWriteMask = Desc.bEnableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL
                                               : D3D11_DEPTH_WRITE_MASK_ZERO,
      .DepthFunc = D3D11_COMPARISON_LESS,
  };

  Result = Device->CreateDepthStencilState(&DepthStencilDesc,
                                           &Pipeline->DepthStencilState);
  if (FAILED(Result)) {
    return nullptr;
  }

  // 블렌드 상태 생성
  D3D11_BLEND_DESC BlendDesc{};
  BlendDesc.AlphaToCoverageEnable = false;
  BlendDesc.IndependentBlendEnable = false;
  auto &RenderTargetBlend = BlendDesc.RenderTarget[0];

  if (Desc.bAdditiveBlend) {
    RenderTargetBlend.BlendEnable = true;
    RenderTargetBlend.SrcBlend = D3D11_BLEND_ONE;
    RenderTargetBlend.DestBlend = D3D11_BLEND_ONE;
    RenderTargetBlend.BlendOp = D3D11_BLEND_OP_ADD;
    RenderTargetBlend.SrcBlendAlpha = D3D11_BLEND_ONE;
    RenderTargetBlend.DestBlendAlpha = D3D11_BLEND_ZERO;
    RenderTargetBlend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    RenderTargetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  } else {
    RenderTargetBlend.BlendEnable = false;
    RenderTargetBlend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  }

  Result = Device->CreateBlendState(&BlendDesc, &Pipeline->BlendState);
  if (FAILED(Result)) {
    return nullptr;
  }

  D3D11_SAMPLER_DESC SamplerDesc{
      .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
      .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
      .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
      .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
      .ComparisonFunc = D3D11_COMPARISON_NEVER,
      .MaxLOD = D3D11_FLOAT32_MAX,
  };

  Result = Device->CreateSamplerState(&SamplerDesc, &Pipeline->SamplerState);
  if (FAILED(Result)) {
    return nullptr;
  }

  return Pipeline;
}

TSharedPtr<FTexture> FRenderer::CreateTexture(FTextureDesc &desc) {
  auto Texture = TSharedPtr<FTexture>{new FTexture()};

  D3D11_TEXTURE2D_DESC TextureDesc = {
      .Width = desc.Width,
      .Height = desc.Height,
      .MipLevels = 1u,
      .ArraySize = 1u,
      .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
      .SampleDesc = {.Count = 1u},
      .Usage = D3D11_USAGE_DEFAULT,
      .BindFlags = D3D11_BIND_SHADER_RESOURCE,
  };

  D3D11_SUBRESOURCE_DATA InitialData = {
      .pSysMem = desc.PixelData,
      .SysMemPitch = desc.RowPitch,
  };

  HRESULT Result =
      Device->CreateTexture2D(&TextureDesc, &InitialData, &Texture->Texture2D);
  if (FAILED(Result)) {
    return nullptr;
  }

  Result = Device->CreateShaderResourceView(Texture->Texture2D.Get(), nullptr,
                                            &Texture->TextureSRV);
  if (FAILED(Result)) {
    return nullptr;
  }

  Texture->Width = desc.Width;
  Texture->Height = desc.Height;

  return Texture;
}

TSharedPtr<FRenderPipeline> FRenderer::GetPipeline(EPipelineID Id) const {
  return FRenderResourceLibrary::Get().GetPipeline(Id);
}

bool FRenderer::InitializeDeviceAndSwapChain(HWND Window) {
  constexpr D3D_FEATURE_LEVEL FeatureLevels[] = {D3D_FEATURE_LEVEL_11_0};

  DXGI_SWAP_CHAIN_DESC SwapChainDesc{
      .BufferDesc =
          {
              .Width = 0u,
              .Height = 0u,
              .Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
          },
      .SampleDesc =
          {
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
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, CreateDeviceFlags,
      FeatureLevels, ARRAYSIZE(FeatureLevels), D3D11_SDK_VERSION,
      &SwapChainDesc, &SwapChain, &Device, nullptr, &Context);
  if (FAILED(Result)) {
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

bool FRenderer::InitializeBackBufferAndDepthStencil() {
  Microsoft::WRL::ComPtr<ID3D11Texture2D> BackBuffer;
  HRESULT Result = SwapChain->GetBuffer(0, IID_PPV_ARGS(&BackBuffer));
  if (FAILED(Result)) {
    return false;
  }

  Result =
      Device->CreateRenderTargetView(BackBuffer.Get(), nullptr, &BackBufferRTV);
  if (FAILED(Result)) {
    return false;
  }

  D3D11_TEXTURE2D_DESC DepthStencilDesc = {
      .Width = static_cast<UINT>(Viewport.Width),
      .Height = static_cast<UINT>(Viewport.Height),
      .MipLevels = 1u,
      .ArraySize = 1u,
      .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
      .SampleDesc =
          {
              .Count = 1u,
          },
      .Usage = D3D11_USAGE_DEFAULT,
      .BindFlags = D3D11_BIND_DEPTH_STENCIL,
  };

  Result =
      Device->CreateTexture2D(&DepthStencilDesc, nullptr, &DepthStencilBuffer);
  if (FAILED(Result)) {
    return false;
  }

  Result = Device->CreateDepthStencilView(DepthStencilBuffer.Get(), nullptr,
                                          &DepthStencilView);
  if (FAILED(Result)) {
    return false;
  }

  return true;
}

bool FRenderer::InitializeConstantBuffers()
{
  // b0를 쓰는 모든 상수 타입이 공유하는 버퍼.
  // 가장 큰 구조체보다 크게 잡아두고, 초과 여부는 UpdateBuffer의 static_assert가 잡는다.
  D3D11_BUFFER_DESC b0Desc = {
      .ByteWidth = ConstantBufferSize,
      .Usage = D3D11_USAGE_DYNAMIC,
      .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
      .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
  };

  HRESULT Result = Device->CreateBuffer(&b0Desc, nullptr, &b0ConstantBuffer);
  if (FAILED(Result)) {
    return false;
  }

  D3D11_BUFFER_DESC FrameConstantBufferDesc = {
      .ByteWidth = sizeof(FFrameConstants),
      .Usage = D3D11_USAGE_DEFAULT,
      .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
  };

  Result = Device->CreateBuffer(&FrameConstantBufferDesc, nullptr,
                                &FrameConstantBuffer);


  if (FAILED(Result)) {
      return false;
  }

  D3D11_BUFFER_DESC lightbufferDesc = {
      .ByteWidth = sizeof(FLightConstants),
      .Usage = D3D11_USAGE_DEFAULT,
      .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
  };



  Result = Device->CreateBuffer(&lightbufferDesc, nullptr, &LightConstantBuffer);

  if (FAILED(Result)) {
      return false;
  }





  return true;
}

void FRenderer::UpdateLightConstants(const FLightConstants& Constants)
{
    Context->UpdateSubresource(LightConstantBuffer.Get(), 0, nullptr, &Constants, 0, 0);
    Context->PSSetConstantBuffers(2, 1, LightConstantBuffer.GetAddressOf());
}