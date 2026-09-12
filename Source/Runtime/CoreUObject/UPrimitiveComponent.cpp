#include "UPrimitiveComponent.h"
#include "UObjectGlobals.h" 
#include "UClass.h"
#include "Runtime/Engine/UScene.h"

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)

void UPrimitiveComponent::Register(UScene& InScene)
{
	if (!PrimitiveMesh || !PrimitiveMaterial) { return; }

	Super::Register(InScene);
	InScene.AddRenderComponent(this);
}

void UPrimitiveComponent::Unregister()
{
	if (Scene)
	{
		Scene->RemoveRenderComponent(this);
	}

	Super::Unregister();
}

void UPrimitiveComponent::SetRelativeTransform(const FTransform& RelativeTransform)
{
	Super::SetRelativeTransform(RelativeTransform);	
}
