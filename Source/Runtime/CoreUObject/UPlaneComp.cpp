#include "UPlaneComp.h"

#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"
#include "Runtime/Engine/UScene.h"

IMPLEMENT_UCLASS(UPlaneComp, UPrimitiveComponent)
UCLASS_META(UPlaneComp, DisplayName, "Plane")
UCLASS_META(UPlaneComp, MeshName, "Plane")

void UPlaneComp::Register(UScene& InScene)
{
	FRenderResourceLibrary* Resources = InScene.GetRenderResourceLibrary();
	SetMesh(Resources ? Resources->GetMesh("Plane") : nullptr);
	SetMaterial(Resources ? Resources->GetMaterial("Simple") : nullptr);
	Super::Register(InScene);
}

