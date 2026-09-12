#include "UCylinderComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UCylinderComp, UPrimitiveComponent)
UCLASS_META(UCylinderComp, DisplayName, "Cylinder")
UCLASS_META(UCylinderComp, MeshName, "Cylinder")

void UCylinderComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	auto& ResLib = FRenderResourceLibrary::Get();
	SetMesh(ResLib.GetMesh("Cylinder"));
	SetMaterial(ResLib.GetMaterial("Simple"));
}
