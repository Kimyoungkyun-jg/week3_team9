#include "AActor.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/CoreUObject/UClass.h"
#include "Runtime/CoreUObject/FReferenceCollector.h"
#include "Runtime/CoreUObject/USceneComponent.h"

IMPLEMENT_UCLASS(AActor, UObject)

void AActor::AddComponent(USceneComponent* Addcomp)
{
	if (Addcomp == nullptr)
	{
		return;
	}

	if (RootComponent == nullptr)
	{
		RootComponent = Addcomp;
	}

	AttachedComp.push_back(Addcomp);
}

void AActor::AddReferencedObjects(FReferenceCollector& Collector)
{
	UObject::AddReferencedObjects(Collector);

	if (RootComponent)
	{
		Collector.AddReferencedObject(RootComponent);
	}

	for (USceneComponent* Component : AttachedComp)
	{
		Collector.AddReferencedObject(Component);
	}
}

void AActor::Update(float DeltaTime)
{
	for (USceneComponent* Component : AttachedComp)
	{
		if (Component)
		{
			Component->Update(DeltaTime);
		}
	}
}
