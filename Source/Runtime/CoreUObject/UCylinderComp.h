#pragma once

#include "UPrimitiveComponent.h"
#include "Runtime/Engine/UScene.h"

class UCylinderComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UCylinderComp, UPrimitiveComponent)
	GENERATED_BODY()

protected:

	void OnRegister(UScene& Scene) override;
};

inline void UCylinderComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	SetMesh(Scene.GetRenderResourceLibrary().GetCylinderMesh());
	SetMaterial(Scene.GetRenderResourceLibrary().GetSimpleMaterial());
}
