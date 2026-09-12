#include "UCubeComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UCubeComp, UPrimitiveComponent)
UCLASS_META(UCubeComp, DisplayName, "Cube")
UCLASS_META(UCubeComp, MeshName, "Cube")

void UCubeComp::Initialize(UObject* Context)
{
	Super::Initialize(Context);

	if (!Scene) { return; }
	SetMesh(Scene->GetRenderResourceLibrary()->GetMesh("Cube"));
	SetMaterial(Scene->GetRenderResourceLibrary()->GetMaterial("Textured"));
}
