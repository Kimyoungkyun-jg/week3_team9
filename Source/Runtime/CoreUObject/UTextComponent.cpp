#include "UTextComponent.h"
#include "UClass.h"
#include "Runtime/Engine/UScene.h"

IMPLEMENT_UCLASS(UTextComponent, UBillBoardComp)
UCLASS_META(UTextComponent, DisplayName, "Text")
UCLASS_META(UTextComponent, MeshName, "Text")

void UTextComponent::OnRegister(UScene& Scene)
{
	Super::OnRegister(Scene);

	SetMesh(Scene.GetRenderResourceLibrary().GetMesh("Text"));
	SetMaterial(Scene.GetRenderResourceLibrary().GetMaterial("Text"));
}

void UTextComponent::RebuildTextMesh()
{
	// TODO: 라이브러리의 create 함수 내부 로직 옮겨와야 함	
}
