#include "UTextComponent.h"
#include "UClass.h"
#include "Runtime/Engine/UScene.h"

IMPLEMENT_UCLASS(UTextComponent, UBillBoardComp)
UCLASS_META(UTextComponent, DisplayName, "Text")
UCLASS_META(UTextComponent, MeshName, "Text")

void UTextComponent::Register(UScene& Scene)
{
	FRenderResourceLibrary* Resources = Scene.GetRenderResourceLibrary();
	SetMesh(Resources ? Resources->GetMesh("Text") : nullptr);
	SetMaterial(Resources ? Resources->GetMaterial("Text") : nullptr);
	Super::Register(Scene);
}

void UTextComponent::RebuildTextMesh()
{
	// TODO: 라이브러리의 create 함수 내부 로직 옮겨와야 함	

	// 여기서 렌더러의 devicecontext든 뭐 가져와서 버텍스버퍼 동적으로 수정
	// 여기서 프리미티브 겟메시 해서 업데이트버텍스버퍼(메시->버텍스버퍼) 이런게있음 될듯
}
