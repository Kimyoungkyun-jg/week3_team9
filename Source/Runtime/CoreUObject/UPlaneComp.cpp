#include "UPlaneComp.h"

#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"
#include "Runtime/Engine/UScene.h"

IMPLEMENT_UCLASS(UPlaneComp, UPrimitiveComponent)
UCLASS_META(UPlaneComp, DisplayName, "Plane")
UCLASS_META(UPlaneComp, MeshName, "Plane")

void UPlaneComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	SetMesh(Scene.GetRenderResourceLibrary().GetPlaneMesh());
	SetMaterial(Scene.GetRenderResourceLibrary().GetSimpleMaterial());
}
