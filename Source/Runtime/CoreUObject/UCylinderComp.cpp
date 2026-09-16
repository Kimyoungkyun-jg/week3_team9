#include "UCylinderComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UCylinderComp, UPrimitiveComponent)
UCLASS_META(UCylinderComp, DisplayName, "Cylinder")
UCLASS_META(UCylinderComp, MeshName, "Cylinder")

void UCylinderComp::Register(UScene &InScene) {
  FRenderResourceLibrary *Resources = InScene.GetRenderResourceLibrary();
  SetMesh(Resources ? Resources->GetMesh(FName("Cylinder")) : nullptr);
  SetMaterial(Resources ? Resources->GetMaterial(FName("Simple")) : nullptr);
  Super::Register(InScene);
}
