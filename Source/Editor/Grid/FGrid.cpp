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
	GridMesh = RenderResources.GetMesh("Grid");
	GridMaterial = RenderResources.GetMaterial("Grid");
	LineMesh = RenderResources.GetMesh("Arrow");
	LineMaterial = RenderResources.GetMaterial("Simple");
}

void FGrid::Draw(FRenderer& Renderer, const FCamera& Camera)
{
	if (!GridMesh || !GridMaterial) return;

	// 카메라 XY 따라감, Z=0 (바닥)
	const float ScaleFactor = (CellSize > 1.0f) ? CellSize : 1.0f;
	const FMatrix World =
		FMatrix::MakeScale(FVector{ ScaleFactor, ScaleFactor, 1.0f }) *
		FMatrix::MakeTranslation(FVector{ Camera.Position.X, Camera.Position.Y, 0.0f });

	const FMatrix VP = Camera.CreateViewProjectionMatrix();

	FGridConstants C;
	C.World = World;
	C.MVP = World * VP;  // 스왑은 UpdateGridConstants 가 함
	C.CellSize = CellSize;

	Renderer.Draw(*GridMesh, *GridMaterial, C, false);

	if (!LineMesh || !LineMaterial) return;

	const FMatrix LineMatrix = FMatrix::MakeTranslation(FVector{ -0.5f, 0.0f, 0.0f }) * FMatrix::MakeScale(FVector{ 100.0f * ScaleFactor, 0.5f * ScaleFactor, 0.5f * ScaleFactor });
	Renderer.Draw<FObjectConstants>(*LineMesh, *LineMaterial, { LineMatrix * FMatrix::MakeTranslation(FVector{ Camera.Position.X, 0.0f, 0.0f }) * VP, FVector{ 1.0f, 0.0f, 0.0f }, 1.0f });
	Renderer.Draw<FObjectConstants>(*LineMesh, *LineMaterial, { LineMatrix * FMatrix::MakeRotationZ(std::numbers::pi_v<float> * 0.5f) * FMatrix::MakeTranslation(FVector{ 0.0f, Camera.Position.Y, 0.0f }) * VP, FVector{ 0.0f, 1.0f, 0.0f }, 1.0f });
	Renderer.Draw<FObjectConstants>(*LineMesh, *LineMaterial, { LineMatrix * FMatrix::MakeRotationY(std::numbers::pi_v<float> * 0.5f) * FMatrix::MakeTranslation(FVector{ 0.0f, 0.0f, Camera.Position.Z }) * VP, FVector{ 0.0f, 0.0f, 1.0f }, 1.0f });
}

void FGrid::DrawLine(FRenderer& Renderer, const FCamera& Camera)
{
    auto& LineBatcher = Renderer.GetLineBatcher();
    //그리드 범위 설정
    
    const int32 HalfLineCount = 50; // 앞뒤로 50개씩 (총 100x100 격자)
    const float Extent = HalfLineCount * CellSize;
    
    //카메라 위치에 맞춘 기준 중심점 (카메라가 이동해도 격자가 부드럽게 유지됨)
    const float SnapX = std::floor(Camera.Position.X / CellSize) * CellSize;
    const float SnapY = std::floor(Camera.Position.Y / CellSize) * CellSize;
    const FVector4 GridColor{ 0.25f, 0.25f, 0.25f, 1.0f }; // 어두운 회색 격자선
    const FVector4 AxisColorX{ 0.8f, 0.2f, 0.2f, 1.0f };   // 빨간색 X축
    const FVector4 AxisColorY{ 0.2f, 0.8f, 0.2f, 1.0f };   // 초록색 Y축
    
    //가로선 (X축 방향 선들)
    
    for (int32 i = -HalfLineCount; i <= HalfLineCount; ++i)
    {
        float Y = SnapY + i * CellSize;
        FVector4 Color = (std::abs(Y) < 0.001f) ? AxisColorX : GridColor; // Y=0은 X축선
        LineBatcher.DrawLine(
            FVector{ SnapX - Extent, Y, 0.0f },
            FVector{ SnapX + Extent, Y, 0.0f },
            Color
        );
    }
    //세로선 (Y축 방향 선들)
    for (int32 i = -HalfLineCount; i <= HalfLineCount; ++i)
    {
        float X = SnapX + i * CellSize;
        FVector4 Color = (std::abs(X) < 0.001f) ? AxisColorY : GridColor; // X=0은 Y축선
        LineBatcher.DrawLine(
            FVector{ X, SnapY - Extent, 0.0f },
            FVector{ X, SnapY + Extent, 0.0f },
            Color
        );
    }
    //Z축 선 (필요 시 파란색 수직선)
    LineBatcher.DrawLine(FVector{ 0.0f, 0.0f, -Extent }, FVector{ 0.0f, 0.0f, Extent }, FVector4{ 0.2f, 0.4f, 0.9f, 1.0f });
}
