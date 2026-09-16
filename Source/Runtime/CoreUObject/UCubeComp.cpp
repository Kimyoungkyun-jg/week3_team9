#include "UCubeComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UCubeComp, UPrimitiveComponent)
UCLASS_META(UCubeComp, DisplayName, "Cube")
UCLASS_META(UCubeComp, MeshName, "Cube")

void UCubeComp::Register(UScene &InScene) {
  SetMeshID(FName("Cube"));
  SetMaterialID(FName("Textured"));
  Super::Register(InScene);
}
