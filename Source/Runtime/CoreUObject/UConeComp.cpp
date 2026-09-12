#include "UConeComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UConeComp, UPrimitiveComponent)
UCLASS_META(UConeComp, DisplayName, "Cone")
UCLASS_META(UConeComp, MeshName, "Cone")

void UConeComp::Initialize(UObject* Context)
{
	Super::Initialize(Context);

	if (!Scene) { return; }
	SetMesh(Scene->GetRenderResourceLibrary()->GetMesh("Cone"));
	SetMaterial(Scene->GetRenderResourceLibrary()->GetMaterial("Simple"));
}
