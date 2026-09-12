#include "UCylinderComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UCylinderComp, UPrimitiveComponent)
UCLASS_META(UCylinderComp, DisplayName, "Cylinder")
UCLASS_META(UCylinderComp, MeshName, "Cylinder")

void UCylinderComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	SetMesh(Scene.GetRenderResourceLibrary()->GetMesh("Cylinder"));
	SetMaterial(Scene.GetRenderResourceLibrary()->GetMaterial("Simple"));
}
