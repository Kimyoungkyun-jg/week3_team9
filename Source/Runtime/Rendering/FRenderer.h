#pragma once

#include "FMaterial.h"
#include "FMesh.h"
#include "FRenderPipeline.h"
#include "Runtime/Core/IntTypes.h"
#include "Runtime/Core/PointerTypes.h"
#include "Runtime/Core/TMap.h"
#include "Runtime/Math/FVector2.h"
#include "ShaderConstants.h"
#include <Windows.h>
#include <d3d11.h>
#include <filesystem>
#include <wrl/client.h>


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
    {EBuiltinPipeline::Textured, L"TexturedVS.cso", L"TexturedPS.cso"},
    {EBuiltinPipeline::Grid, L"GridVS.cso", L"GridPS.cso"},
    {EBuiltinPipeline::RotationGizmo, L"RotationGizmoVS.cso",L"RotationGizmoPS.cso"},
};

class FRenderer final {
public:
  bool Initialize(HWND Window);
  void Shutdown();
  void BeginFrame();
  void SetViewportUV(FVector2 TopLeftUV, FVector2 LengthUV);
  void Draw(const FMesh &Mesh, const FMaterial &Material,
            const FObjectConstants &ObjectConstants);
  void DrawGrid(const FMesh &Mesh, const FMaterial &Material,
                const FGridConstants &GridConstants);
  void ClearDepth();
  void SwapBuffer();
  void OnWindowSize(UINT Width, UINT Height);

  EViewModeIndex GetRenderMode() const { return CurrentRenderMode; }
  void SetRenderMode(EViewModeIndex InMode) { CurrentRenderMode = InMode; }

  [[nodiscard]]
  TSharedPtr<FMesh> CreateMesh(const FMeshDesc &Desc);
  [[nodiscard]]
  TSharedPtr<FMaterial> CreateMaterial(const FMaterialDesc &Desc);

  void GetDeviceAndContext_ImplDX11(ID3D11Device *&DeviceOut,
                                    ID3D11DeviceContext *&ContextOut);

  [[nodiscard]]
  TSharedPtr<FRenderPipeline> CreateRenderPipeline(const FRenderPipelineDesc &Desc,
                                                  EViewModeIndex RenderMode = EViewModeIndex::VMI_Lit);

  // 파이프라인 조회
  [[nodiscard]]
  TSharedPtr<FRenderPipeline> GetPipeline(EBuiltinPipeline Id) const;

  // 파이프라인 보관 맵
  TMap<EBuiltinPipeline, TSharedPtr<FRenderPipeline>> AllPipelineMap;

private:
  bool InitializeDeviceAndSwapChain(HWND Window);
  bool InitializeBackBufferAndDepthStencil();
  bool InitializeConstantBuffers();
  bool InitializePipeLines();
  bool CreateSolidWireframePipeline();

  // 그리드 상수 버퍼 초기화
  bool InitializeGridConstantBuffers();
  void UpdateObjectConstants(const FObjectConstants &Constants);
  // 그리드 상수 버퍼 업데이트
  void UpdateGridConstants(const FGridConstants &Constants);

private:
  Microsoft::WRL::ComPtr<ID3D11Device> Device;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;
  Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
  D3D11_VIEWPORT Viewport{};

  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> BackBufferRTV;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilBuffer;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;

  Microsoft::WRL::ComPtr<ID3D11Buffer> ObjectConstantBuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer> GridConstantBuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer> FrameConstantBuffer;

  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> EditorViewPortRTV;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> EditorViewPortSRV;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTexture;

  EViewModeIndex CurrentRenderMode = EViewModeIndex::VMI_Lit;
};
