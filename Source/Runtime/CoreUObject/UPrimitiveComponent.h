#pragma once

#include "USceneComponent.h"
#include "Runtime/Core/PointerTypes.h"
#include "Runtime/Rendering/FMaterial.h"
#include "Runtime/Rendering/FMesh.h"

class UPrimitiveComponent : public USceneComponent
{
	GENERATED_BODY()
	DECLARE_UCLASS(UPrimitiveComponent, USceneComponent)

public:
	[[nodiscard]] TSharedPtr<FMesh> GetMesh() const { return PrimitiveMesh; }
	[[nodiscard]] TSharedPtr<FMaterial> GetMaterial() const { return PrimitiveMaterial; }
	[[nodiscard]] FMatrix GetModelMatrix() const { return RelativeTransform.ToMatrix(); }

	// 메쉬 및 재질 설정
	void SetMesh(TSharedPtr<FMesh> Mesh) { PrimitiveMesh = std::move(Mesh); }
	void SetMaterial(TSharedPtr<FMaterial> Material) { PrimitiveMaterial = std::move(Material); }

protected:
	UPrimitiveComponent() = default;

private:
	TSharedPtr<FMesh> PrimitiveMesh;
	TSharedPtr<FMaterial> PrimitiveMaterial;
};
