#include "FGrid.h"

#include "Runtime/Core/IntTypes.h"
#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/Engine/FCamera.h"
#include "Runtime/Rendering/FRenderer.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "Runtime/Math/FMatrix.h"
#include "Runtime/Math/FVector4.h"
#include "Editor/Core/FEditor.h"
#include <numbers>

#include "Runtime/Engine/FRayCastingManager.h"


void FGrid::Initialize(FRenderResourceLibrary& RenderResources)
{
	GridMesh = RenderResources.GetGridMesh();
	GridMaterial = RenderResources.GetGridMaterial();
	LineMesh = RenderResources.GetArrowMesh();
	LineMaterial = RenderResources.GetSimpleMaterial();
}

void FGrid::Draw(FRenderer& Renderer, const FCamera& Camera)
{
	if (!GridMesh || !GridMaterial) return;

	// 카메라 XY 따라감, Z=0 (바닥)
	const FMatrix World =
		FMatrix::MakeTranslation(FVector{ Camera.Position.X, Camera.Position.Y, 0.0f });

	const FMatrix VP = Camera.CreateViewProjectionMatrix();

	FGridConstants C;
	C.World = World;
	C.MVP = World * VP;  // 스왑은 UpdateGridConstants 가 함
	C.CellSize = 1.0f;

	Renderer.DrawGrid(*GridMesh, *GridMaterial, C);

	if (!LineMesh || !LineMaterial) return;

	const FMatrix LineMatrix = FMatrix::MakeTranslation(FVector{ -0.5f, 0.0f, 0.0f }) * FMatrix::MakeScale(FVector{ 100.0f, 0.5f, 0.5f });
	Renderer.Draw(*LineMesh, *LineMaterial, { LineMatrix * FMatrix::MakeTranslation(FVector{ Camera.Position.X, 0.0f, 0.0f }) * VP, FVector{ 1.0f, 0.0f, 0.0f }, 1.0f });
	Renderer.Draw(*LineMesh, *LineMaterial, { LineMatrix * FMatrix::MakeRotationZ(std::numbers::pi_v<float> * 0.5f) * FMatrix::MakeTranslation(FVector{ 0.0f, Camera.Position.Y, 0.0f }) * VP, FVector{ 0.0f, 1.0f, 0.0f }, 1.0f });
	Renderer.Draw(*LineMesh, *LineMaterial, { LineMatrix * FMatrix::MakeRotationY(std::numbers::pi_v<float> * 0.5f) * FMatrix::MakeTranslation(FVector{ 0.0f, 0.0f, Camera.Position.Z }) * VP, FVector{ 0.0f, 0.0f, 1.0f }, 1.0f });
}