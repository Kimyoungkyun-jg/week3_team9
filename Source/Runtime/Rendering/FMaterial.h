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

private:
	void BindResources(ID3D11DeviceContext& Context) const;

	TSharedPtr<FRenderPipeline> Pipeline;
};

struct FMaterialDesc
{
	FWString VertexShaderFileName;
	FWString PixelShaderFileName;
	bool bEnableDepthTest = true;
};