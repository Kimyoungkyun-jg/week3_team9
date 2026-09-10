#pragma once

#include "FMesh.h"
#include "FMaterial.h"
#include "Vertices.h"
#include "Runtime/Core/PointerTypes.h"
#include "Runtime/Core/TMap.h"
#include "Runtime/Core/FString.h"
#include "Runtime/Core/TArray.h"

class FRenderer;

class FRenderResourceLibrary final
{
public:
	bool Initialize(FRenderer& Renderer);

	// 메쉬 보관 맵
	TMap<FString, TSharedPtr<FMesh>> AllMeshMap;
	// 머티리얼 보관 맵
	TMap<FString, TSharedPtr<FMaterial>> AllMaterialMap;

	// 메쉬 조회
	TSharedPtr<FMesh> GetMesh(const FString& name) const
	{
		auto it = AllMeshMap.find(name);
		if (it != AllMeshMap.end())
			return it->second;
		return nullptr;
	}

	// 메쉬 등록
	TSharedPtr<FMesh> RegisterMesh(const FString& name, TSharedPtr<FMesh> inMesh)
	{
		AllMeshMap[name] = inMesh;
		return inMesh;
	}

	// 머티리얼 조회
	TSharedPtr<FMaterial> GetMaterial(const FString& name) const
	{
		auto it = AllMaterialMap.find(name);
		if (it != AllMaterialMap.end())
			return it->second;
		return nullptr;
	}

	// 머티리얼 등록
	TSharedPtr<FMaterial> RegisterMaterial(const FString& name, TSharedPtr<FMaterial> inMaterial)
	{
		AllMaterialMap[name] = inMaterial;
		return inMaterial;
	}

	// 메쉬 전체 해제
	void DestroyAllMeshes()
	{
		AllMeshMap.clear();
	}

	// 정점 배열 메쉬 캐싱 생성
	TSharedPtr<FMesh> GetOrCreateMesh(const FString& name, const TArray<FVertexData>& vertices);

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

	FRenderer* RendererRef = nullptr;
};

#include "FRenderer.h"

inline TSharedPtr<FMesh> FRenderResourceLibrary::GetOrCreateMesh(const FString& name, const TArray<FVertexData>& vertices)
{
	auto it = AllMeshMap.find(name);
	if (it != AllMeshMap.end())
		return it->second;

	FMeshDesc Desc{
		.VertexData = vertices.data(),
		.VertexDataSize = static_cast<uint32>(sizeof(FVertexData) * vertices.size()),
		.VertexStride = static_cast<uint32>(sizeof(FVertexData)),
		.VertexCount = static_cast<uint32>(vertices.size())
	};
	TSharedPtr<FMesh> newMesh = RendererRef ? RendererRef->CreateMesh(Desc) : nullptr;
	if (newMesh)
	{
		AllMeshMap[name] = newMesh;
	}
	return newMesh;
}