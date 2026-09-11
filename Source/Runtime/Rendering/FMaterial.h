#pragma once

#include "FRenderPipeline.h"
#include "Runtime/Core/FString.h"
#include "Runtime/Core/PointerTypes.h"
#include "Vertices.h"
#include <d3d11.h>

class FRenderer;

class FMaterial final {
  friend class FRenderer;

public:
  FMaterial() = default;
  
  void SetPipeLine(const TSharedPtr<FRenderPipeline>& InPipeline);
  void SetWireframePipeLine(const TSharedPtr<FRenderPipeline>& InPipeline);

  [[nodiscard]] TSharedPtr<FRenderPipeline> GetPipeline() const { return Pipeline; }
  [[nodiscard]] TSharedPtr<FRenderPipeline> GetWireframePipeline() const { return WireframePipeline; }

private:
  void BindResources(ID3D11DeviceContext &Context) const;

  TSharedPtr<FRenderPipeline> Pipeline;
  TSharedPtr<FRenderPipeline> WireframePipeline;
};

struct FMaterialDesc {
  FWString VertexShaderFileName;
  FWString PixelShaderFileName;
  bool bEnableDepthTest = true;
};