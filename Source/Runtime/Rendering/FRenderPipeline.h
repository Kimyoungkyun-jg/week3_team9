#pragma once

#include "Vertices.h"
#include <d3d11.h>
#include <wrl/client.h>
#include "Runtime/Core/FString.h"

struct FRenderPipelineDesc {
  FWString VertexShaderFileName;
  FWString PixelShaderFileName;
  bool bEnableDepthTest = true;

  bool operator==(const FRenderPipelineDesc &) const = default;
};

class FRenderPipeline final {
  friend class FRenderer;
  friend class FLineBatcher;

public:
  [[nodiscard]] FRenderPipelineDesc GetPipelineDesc() const { return desc; }

private:
  FRenderPipelineDesc desc;

  void Bind(ID3D11DeviceContext &Context) const;

  Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;

  Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
};