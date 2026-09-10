#pragma once
#pragma once

#include "UPrimitiveComponent.h"
#include "Runtime/Engine/UScene.h"

class UConeComp : public UPrimitiveComponent
{
	GENERATED_BODY()

protected:
	explicit UConeComp() = default;

	void OnRegister(UScene& Scene) override;
};

inline void UConeComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	SetMesh(Scene.GetRenderResourceLibrary().GetConeMesh());
	SetMaterial(Scene.GetRenderResourceLibrary().GetSimpleMaterial());
}
