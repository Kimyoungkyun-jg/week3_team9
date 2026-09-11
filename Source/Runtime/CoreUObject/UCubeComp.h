#pragma once

#include "UPrimitiveComponent.h"
#include "Runtime/Engine/UScene.h"

class UCubeComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UCubeComp, UPrimitiveComponent)
	GENERATED_BODY()

protected:
	explicit UCubeComp() = default;

	void OnRegister(UScene& Scene) override;
};

inline void UCubeComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);
	
	SetMesh(Scene.GetRenderResourceLibrary().GetMesh("Cube"));
	SetMaterial(Scene.GetRenderResourceLibrary().GetMaterial("Simple"));
}
