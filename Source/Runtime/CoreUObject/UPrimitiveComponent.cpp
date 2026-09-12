#include "UPrimitiveComponent.h"
#include "UObjectGlobals.h" 
#include "UClass.h"
#include "Runtime/Engine/UScene.h"

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)

void UPrimitiveComponent::Initialize(UObject* Context)
{
	Super::Initialize(Context);
	if (Scene)
	{
		Scene->AddRenderComponent(this);
	}
}

void UPrimitiveComponent::Release()
{
	if (Scene)
	{
		Scene->RemoveRenderComponent(this);
		Scene = nullptr;
	}

	Super::Release();
}

void UPrimitiveComponent::SetRelativeTransform(const FTransform& RelativeTransform)
{
	Super::SetRelativeTransform(RelativeTransform);	
}
