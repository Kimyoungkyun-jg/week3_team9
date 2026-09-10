#pragma once

#include "FRenderPipeline.h"
#include "Vertices.h"
#include "Runtime/Core/FString.h"
#include "Runtime/Core/PointerTypes.h"
#include <d3d11.h>

class FRenderer;

class FMaterial final
{
	friend class FRenderer;

public:
	[[nodiscard]] EVertexLayout GetVertexLayout() const { return Pipeline->GetVertexLayout(); }

private:
	FMaterial() = default;

	void BindResources(ID3D11DeviceContext& Context) const;

	TSharedPtr<FRenderPipeline> Pipeline;
};

struct FMaterialDesc
{
	FWString VertexShaderFileName;
	FWString PixelShaderFileName;
	EVertexLayout VertexLayout = EVertexLayout::None;
	bool bEnableDepthTest = true;
	// Rasterizer Mode, Blend Mode, Depth Mode 등 추가 가능 (Pipeline에 등록)
	// TextureSRV, Sampler, Material Constant Buffer 등 추가 가능
};
