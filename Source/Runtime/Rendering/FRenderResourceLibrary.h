#pragma once

#include "FMesh.h"
#include "FMaterial.h"
#include "Runtime/Core/PointerTypes.h"
#include "../Core/TMap.h"

class FRenderer;

class FRenderResourceLibrary final
{
public:
	bool Initialize(FRenderer& Renderer);

	// TODO: 필요할 시 Enum 기반 switch-case로 처리
	//  GetConstCubeMesh() -> const FMesh를 반환, GetCubeMesh() -> FMesh를 생성해서 반환으로 나눌 수 있음
	//  지금은 어차피 FMesh로 반환해도 Getter만 있어서 수정 못 함
	//  UPrimitive가 비const FMesh를 소유하도록 되어 있어서 이렇게 둠

	[[nodiscard]] TSharedPtr<FMaterial> GetMaterial(const FString& InName) { return MaterialTable.at(InName); }
	[[nodiscard]] TSharedPtr<FMesh> GetMesh(const FString& InName) { return MeshTable.at(InName); }

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

	// TODO: FName 으로 빠른 compare 가능하도록 수정 필요
	TMap<FString, TSharedPtr<FMesh>> MeshTable;
	TMap<FString, TSharedPtr<FMaterial>> MaterialTable;
};
