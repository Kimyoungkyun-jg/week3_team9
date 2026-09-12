#include "UPlaneComp.h"

#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"
#include "Runtime/Engine/UScene.h"

IMPLEMENT_UCLASS(UPlaneComp, UPrimitiveComponent)
UCLASS_META(UPlaneComp, DisplayName, "Plane")
UCLASS_META(UPlaneComp, MeshName, "Plane")

void UPlaneComp::Initialize(UObject* Context)
{
	Super::Initialize(Context);

	if (!Scene) { return; }
	SetMesh(Scene->GetRenderResourceLibrary()->GetMesh("Plane"));
	SetMaterial(Scene->GetRenderResourceLibrary()->GetMaterial("Simple"));
}

