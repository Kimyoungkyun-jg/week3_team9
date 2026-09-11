#pragma once
#include "Runtime/Core/IntTypes.h"
#include "Runtime/Core/PointerTypes.h"
#include "Runtime/Rendering/FMesh.h"
#include "Runtime/Rendering/FMaterial.h"

class FRenderResourceLibrary;
struct FCamera;

class FGrid
{
private:
	TSharedPtr<FMesh> GridMesh;
	TSharedPtr<FMaterial> GridMaterial;
	TSharedPtr<FMesh> LineMesh;
	TSharedPtr<FMaterial> LineMaterial;
	float CellSize = 1.0f;

public:
	void Draw(FRenderer& Renderer, const FCamera& Camera);
	void Initialize(FRenderResourceLibrary& RenderResources);

	float GetCellSize() const { return CellSize; }
	void SetCellSize(float InCellSize) { CellSize = (InCellSize > 0.01f) ? InCellSize : 0.01f; }
	float& GetCellSizeRef() { return CellSize; }
};