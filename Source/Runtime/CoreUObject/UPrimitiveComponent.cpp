#include "UPrimitiveComponent.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "UObjectGlobals.h" 
#include "UClass.h"

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)

void UPrimitiveComponent::SetRelativeTransform(const FTransform& RelativeTransform)
{
	Super::SetRelativeTransform(RelativeTransform);	
}

bool UPrimitiveComponent::SetTextureByName(const FString& InTextureName)
{
	auto CurrentMat = GetMaterial();
	if (!CurrentMat)
	{
		return false;
	}

	auto& ResLib = FRenderResourceLibrary::Get();

	// 독립 머티리얼 인스턴스 생성 및 텍스처 교체
	auto NewMaterial = TSharedPtr<FMaterial>(new FMaterial());
	NewMaterial->SetPipeLine(CurrentMat->GetPipeline());

	if (!NewMaterial->SetTextureByName(InTextureName, ResLib))
	{
		return false;
	}

	SetMaterial(NewMaterial);
	return true;
}
