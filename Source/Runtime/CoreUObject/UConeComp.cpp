#include "UConeComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UConeComp, UPrimitiveComponent)
UCLASS_META(UConeComp, DisplayName, "Cone")
UCLASS_META(UConeComp, MeshName, "Cone")

void UConeComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	auto& ResLib = FRenderResourceLibrary::Get();
	SetMesh(ResLib.GetMesh("Cone"));
	SetMaterial(ResLib.GetMaterial("Simple"));
}