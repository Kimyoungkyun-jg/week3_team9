#include "UPrimitiveComponent.h"
#include "UObjectGlobals.h" 
#include "UClass.h"

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)

void UPrimitiveComponent::SetRelativeTransform(FTransform& RelativeTransform)
{
	Super::SetRelativeTransform(RelativeTransform);	
}
