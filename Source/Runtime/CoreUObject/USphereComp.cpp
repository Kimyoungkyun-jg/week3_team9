#include "USphereComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"

IMPLEMENT_UCLASS(USphereComp, UPrimitiveComponent)
UCLASS_META(USphereComp, DisplayName, "Sphere")
UCLASS_META(USphereComp, MeshName, "Sphere")

void USphereComp::Register(UScene &InScene) {
  FRenderResourceLibrary *Resources = InScene.GetRenderResourceLibrary();
  SetMesh(Resources ? Resources->GetMesh(FName("Sphere")) : nullptr);
  SetMaterial(Resources ? Resources->GetMaterial(FName("Simple")) : nullptr);
  Super::Register(InScene);
}
