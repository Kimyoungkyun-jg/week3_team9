#include "UConeComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UConeComp, UPrimitiveComponent)
UCLASS_META(UConeComp, DisplayName, "Cone")
UCLASS_META(UConeComp, MeshName, "Cone")

void UConeComp::Register(UScene &InScene) {
  FRenderResourceLibrary *Resources = InScene.GetRenderResourceLibrary();
  SetMeshID(EMeshID::Cone);
  SetMaterialID(EMaterialID::Simple);
  Super::Register(InScene);
}
