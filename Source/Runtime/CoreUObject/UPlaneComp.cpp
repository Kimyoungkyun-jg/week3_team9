#include "UPlaneComp.h"

#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "UClass.h"
#include "Runtime/Engine/UScene.h"

IMPLEMENT_UCLASS(UPlaneComp, UPrimitiveComponent)
UCLASS_META(UPlaneComp, DisplayName, "Plane")
UCLASS_META(UPlaneComp, MeshName, "Plane")

void UPlaneComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	auto& ResLib = FRenderResourceLibrary::Get();
	SetMesh(ResLib.GetMesh("Plane"));
	SetMaterial(ResLib.GetMaterial("Simple"));
}

