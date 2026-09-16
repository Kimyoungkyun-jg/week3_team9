#include "UPlaneComp.h"

#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Engine/UScene.h"
#include "UClass.h"


IMPLEMENT_UCLASS(UPlaneComp, UPrimitiveComponent)
UCLASS_META(UPlaneComp, DisplayName, "Plane")
UCLASS_META(UPlaneComp, MeshName, "Plane")

void UPlaneComp::Register(UScene &InScene) {
  FRenderResourceLibrary *Resources = InScene.GetRenderResourceLibrary();
  SetMeshID(EMeshID::Plane);
  SetMaterialID(EMaterialID::Simple);
  Super::Register(InScene);
}
