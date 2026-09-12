#pragma once

#include "FMaterial.h"
#include "FMesh.h"
#include "FRenderPipeline.h"
#include "Runtime/Core/IntTypes.h"
#include "Runtime/Core/PointerTypes.h"
#include "Runtime/Core/TMap.h"
#include "Runtime/Math/FVector2.h"
#include "ShaderConstants.h"
#include "Runtime/Rendering/FLineBatcher.h"

#include <Windows.h>
#include <d3d11.h>
#include <filesystem>
#include <wrl/client.h>


class FTexture;
struct FTextureDesc;

inline FWString GetExecutableDirectory() {
  wchar_t Buffer[256];
  GetModuleFileNameW(nullptr, Buffer, 256);
  return std::filesystem::path(Buffer).parent_path();
}

#include "Runtime/Engine/ShowFlags.h"

// 내장 파이프라인 종류
enum class EBuiltinPipeline : uint8 {
  Simple_Solid,
  Simple_Wireframe,
  Textured,
  Text,
  Grid,
  RotationGizmo,
  Count
};

// 파이프라인 정보 엔트리
struct FPipelineEntry {
  EBuiltinPipeline Id;
  const wchar_t *VertexShader;
  const wchar_t *PixelShader;
};

// 기본 파이프라인 테이블
constexpr FPipelineEntry pipelineTable[] = {
    {EBuiltinPipeline::Simple_Solid, L"ExampleVS.cso", L"ExamplePS.cso"},
    // VS는 ExampleVS를 그대로 씀. UV를 PS로 넘기도록 수정해 뒀고,
    // VS가 PS보다 많이 출력하는 건 D3D가 허용하므로 다른 파이프라인에 영향 없음
    {EBuiltinPipeline::Textured, L"ExampleVS.cso", L"TexturedPS.cso"},
    {EBuiltinPipeline::Text, L"ExampleVS.cso", L"TextPS.cso"},
    {EBuiltinPipeline::Grid, L"GridVS.cso", L"GridPS.cso"},
    {EBuiltinPipeline::RotationGizmo, L"RotationGizmoVS.cso",L"RotationGizmoPS.cso"},
};

class FRenderer final {
public:
  bool Initialize(HWND Window);
  void Shutdown();
  void BeginFrame();
  void SetViewportUV(FVector2 TopLeftUV, FVector2 LengthUV);
  void ClearDepth();
  void SwapBuffer();
  void OnWindowSize(UINT Width, UINT Height);

  void FlushLineBatch(const FMatrix& ViewProjection);

  EViewModeIndex GetRenderMode() const { return CurrentRenderMode; }
  void SetRenderMode(EViewModeIndex InMode) { CurrentRenderMode = InMode; }

  [[nodiscard]]
  TSharedPtr<FMesh> CreateMesh(const FMeshDesc &Desc);
  [[nodiscard]]
  TSharedPtr<FMesh> CreateDynamicMesh(const FMeshDesc& Desc);   // 텍스트 렌더링용
  [[nodiscard]]
  TSharedPtr<FMaterial> CreateMaterial(const FMaterialDesc &Desc);

  void GetDeviceAndContext_ImplDX11(ID3D11Device *&DeviceOut,
                                    ID3D11DeviceContext *&ContextOut);

  [[nodiscard]]
  TSharedPtr<FRenderPipeline> CreateRenderPipeline(const FRenderPipelineDesc &Desc,
                                                  EViewModeIndex RenderMode = EViewModeIndex::VMI_Lit);
  [[nodiscard]]
  TSharedPtr<FTexture> CreateTexture(FTextureDesc& desc);

  // 파이프라인 조회
  [[nodiscard]]
  TSharedPtr<FRenderPipeline> GetPipeline(EBuiltinPipeline Id) const;

  // 파이프라인 보관 맵
  TMap<EBuiltinPipeline, TSharedPtr<FRenderPipeline>> AllPipelineMap;
  FLineBatcher& GetLineBatcher() { return LineBatcher; }
private:
  bool InitializeDeviceAndSwapChain(HWND Window);
  bool InitializeBackBufferAndDepthStencil();
  bool InitializeConstantBuffers();
  bool InitializePipeLines();
  bool CreateSolidWireframePipeline();

private:
    FLineBatcher LineBatcher;
  Microsoft::WRL::ComPtr<ID3D11Device> Device;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;
  Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
  D3D11_VIEWPORT Viewport{};

  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> BackBufferRTV;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilBuffer;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;

  // b0에 바인딩되는 모든 상수 타입이 공유한다.
  static constexpr UINT ConstantBufferSize = 256u;
  Microsoft::WRL::ComPtr<ID3D11Buffer> b0ConstantBuffer;

  // b1뷰포트 단위. b0와 동시에 바인딩되므로 별도 버퍼가 필요하다.
  Microsoft::WRL::ComPtr<ID3D11Buffer> FrameConstantBuffer;

  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> EditorViewPortRTV;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> EditorViewPortSRV;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTexture;

  EViewModeIndex CurrentRenderMode = EViewModeIndex::VMI_Lit;
public:

  // bApplyViewMode=false면 뷰모드(와이어프레임) 오버라이드를 건너뛴다
  template <typename TConstants>
  void Draw(const FMesh& Mesh, const FMaterial& Material,
      const TConstants& Constants, bool bApplyViewMode = true)
  {
      UpdateBuffer(Constants);

      TSharedPtr<FRenderPipeline> Pipeline = Material.Pipeline;
      if (bApplyViewMode && CurrentRenderMode == EViewModeIndex::VMI_Wireframe) {
          Pipeline = GetPipeline(EBuiltinPipeline::Simple_Wireframe);
      }
      if (Pipeline) {
          Pipeline->Bind(*Context.Get());
      }

      Material.BindResources(*Context.Get());
      Mesh.BindResources(*Context.Get());

      if (Mesh.HasIndices()) {
          Context->DrawIndexed(Mesh.IndexCount, 0, 0);
      }
      else {
          Context->Draw(Mesh.VertexCount, 0);
      }
  }
private:
  // 어느 상수 타입이든 b0 버퍼 하나에 써 넣는다.
  // 크기가 맞는지는 컴파일 타임에 검사한다.
  template <typename TConstants>
  void UpdateBuffer(const TConstants& Constants)
  {
      static_assert(sizeof(TConstants) <= ConstantBufferSize);
      static_assert(sizeof(TConstants) % 16 == 0);

      // 언리얼 Clip -> D3D Clip 좌표 변환.
      // MVP를 가진 상수 타입에만 적용한다(없는 타입은 그대로 통과).
      TConstants ShaderConstants = Constants;
      if constexpr (requires { ShaderConstants.MVP; }) {
          static const FMatrix UnrealClipToD3DClip{
              FVector{0.0f, 0.0f, 1.0f}, FVector{1.0f, 0.0f, 0.0f},
              FVector{0.0f, 1.0f, 0.0f}, FVector{0.0f, 0.0f, 0.0f}};
          ShaderConstants.MVP *= UnrealClipToD3DClip;
      }

      D3D11_MAPPED_SUBRESOURCE Mapped{};
      if (FAILED(Context->Map(b0ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped))) {
          return;
      }
      std::memcpy(Mapped.pData, &ShaderConstants, sizeof(TConstants));
      Context->Unmap(b0ConstantBuffer.Get(), 0);

      Context->VSSetConstantBuffers(0u, 1u, b0ConstantBuffer.GetAddressOf());
      Context->PSSetConstantBuffers(0u, 1u, b0ConstantBuffer.GetAddressOf());
  }
};

