#pragma once

#include "FMesh.h"
#include "FMaterial.h"
#include "Runtime/Core/PointerTypes.h"

class FRenderer;

class FRenderResourceLibrary final
{
public:
	bool Initialize(FRenderer& Renderer);

	// TODO: 필요할 시 Enum 기반 switch-case로 처리
	//  GetConstCubeMesh() -> const FMesh를 반환, GetCubeMesh() -> FMesh를 생성해서 반환으로 나눌 수 있음
	//  지금은 어차피 FMesh로 반환해도 Getter만 있어서 수정 못 함
	//  UPrimitive가 비const FMesh를 소유하도록 되어 있어서 이렇게 둠
	[[nodiscard]] TSharedPtr<FMesh> GetCubeMesh() { return CubeMesh; }
	[[nodiscard]] TSharedPtr<FMesh> GetCylinderMesh() { return CylinderMesh; }
	[[nodiscard]] TSharedPtr<FMesh> GetConeMesh() { return ConeMesh; }
	[[nodiscard]] TSharedPtr<FMesh> GetArrowMesh() { return ArrowMesh; }
	[[nodiscard]] TSharedPtr<FMesh> GetCircleMesh() { return CircleMesh; }
	[[nodiscard]] TSharedPtr<FMesh> GetRotationGizmoMesh() { return RotationGizmoMesh; }
	[[nodiscard]] TSharedPtr<FMesh> GetSquareArrowMesh() { return SquareArrowMesh; }
	[[nodiscard]] TSharedPtr<FMesh> GetGridMesh() { return GridMesh; }
	[[nodiscard]] TSharedPtr<FMesh> GetSphereMesh() { return SphereMesh; }
	[[nodiscard]] TSharedPtr<FMesh> GetLineMesh() { return LineMesh; }
	[[nodiscard]] TSharedPtr<FMesh> GetPlaneMesh() { return PlaneMesh; }
	[[nodiscard]] TSharedPtr<FMaterial> GetSimpleMaterial() { return SimpleMaterial; }
	[[nodiscard]] TSharedPtr<FMaterial> GetGridMaterial() { return GridMaterial; }
	[[nodiscard]] TSharedPtr<FMaterial> GetRotationGizmoMaterial() { return RotationGizmoMaterial; }

private:
	bool CreateCubeMesh(FRenderer& Renderer);
	bool CreateCylinderMesh(FRenderer& Renderer, float Height, uint32 SliceCount, float TopRadius, float BottomRadius);
	bool CreateConeMesh(FRenderer& Renderer);
	bool CreateArrowMesh(FRenderer& Renderer);
	bool CreateCircleMesh(FRenderer& Renderer);
	bool CreateRotationGizmoMesh(FRenderer& Renderer);
	bool CreateSquareArrowMesh(FRenderer& Renderer);
	bool CreateGridMesh(FRenderer& Renderer);
	bool CreateSphereMesh(FRenderer& Renderer);
	bool CreateLineMesh(FRenderer& Renderer);
	bool CreatePlaneMesh(FRenderer& Renderer);
	bool CreateSimpleMaterial(FRenderer& Renderer);
	bool CreateGridMaterial(FRenderer& Renderer);
	bool CreateRotationGizmoMaterial(FRenderer& Renderer);

private:
	TSharedPtr<FMesh> CubeMesh;
	TSharedPtr<FMesh> CylinderMesh;
	TSharedPtr<FMesh> ConeMesh;
	TSharedPtr<FMesh> ArrowMesh;
	TSharedPtr<FMesh> CircleMesh;
	TSharedPtr<FMesh> RotationGizmoMesh;
	TSharedPtr<FMesh> SquareArrowMesh;
	TSharedPtr<FMesh> GridMesh;
	TSharedPtr<FMesh> SphereMesh;
	TSharedPtr<FMesh> LineMesh;
	TSharedPtr<FMesh> PlaneMesh;
	TSharedPtr<FMaterial> SimpleMaterial;
	TSharedPtr<FMaterial> GridMaterial;
	TSharedPtr<FMaterial> RotationGizmoMaterial;
};
