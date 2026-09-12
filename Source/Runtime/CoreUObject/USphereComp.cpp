#include "USphereComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "UClass.h"

IMPLEMENT_UCLASS(USphereComp, UPrimitiveComponent)
UCLASS_META(USphereComp, DisplayName, "Sphere")
UCLASS_META(USphereComp, MeshName, "Sphere")


void USphereComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	auto& ResLib = FRenderResourceLibrary::Get();
	SetMesh(ResLib.GetMesh("Sphere"));
	SetMaterial(ResLib.GetMaterial("Simple"));
}
