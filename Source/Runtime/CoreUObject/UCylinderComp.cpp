#include "UCylinderComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UCylinderComp, UPrimitiveComponent)
UCLASS_META(UCylinderComp, DisplayName, "Cylinder")
UCLASS_META(UCylinderComp, MeshName, "Cylinder")

void UCylinderComp::Initialize(UObject* Context)
{
	Super::Initialize(Context);

	if (!Scene) { return; }
	SetMesh(Scene->GetRenderResourceLibrary()->GetMesh("Cylinder"));
	SetMaterial(Scene->GetRenderResourceLibrary()->GetMaterial("Simple"));
}
