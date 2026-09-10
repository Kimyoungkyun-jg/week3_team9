#pragma once

#include "UPrimitiveComponent.h"
#include "Runtime/Engine/UScene.h"

class USphereComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(USphereComp, UPrimitiveComponent)
	GENERATED_BODY()

protected:

	void OnRegister(UScene& Scene) override;
};

inline void USphereComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	SetMesh(Scene.GetRenderResourceLibrary().GetSphereMesh());
	SetMaterial(Scene.GetRenderResourceLibrary().GetSimpleMaterial());
}
