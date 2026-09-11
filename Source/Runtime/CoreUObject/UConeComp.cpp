#include "UConeComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UConeComp, UPrimitiveComponent)
UCLASS_META(UConeComp, DisplayName, "Cone")
UCLASS_META(UConeComp, MeshName, "Cone")

void UConeComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);


	SetMesh(Scene.GetRenderResourceLibrary().GetMesh("Cone"));
	SetMaterial(Scene.GetRenderResourceLibrary().GetMaterial("Simple"));
}