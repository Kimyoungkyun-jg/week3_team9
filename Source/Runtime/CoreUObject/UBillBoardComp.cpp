#include "UBillBoardComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Engine/UScene.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UBillBoardComp, UPrimitiveComponent)
UCLASS_META(UBillBoardComp, DisplayName, "BillBoard")
UCLASS_META(UBillBoardComp, MeshName, "BillBoard")

void UBillBoardComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	SetMesh(Scene.GetRenderResourceLibrary().GetMesh("Grid"));
	SetMaterial(Scene.GetRenderResourceLibrary().GetMaterial("Textured"));
}
