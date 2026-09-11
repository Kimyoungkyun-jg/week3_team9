#include "AActor.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/CoreUObject/UClass.h"
#include "Runtime/CoreUObject/FReferenceCollector.h"
#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/Engine/UScene.h"


IMPLEMENT_UCLASS(AActor, UObject)

void AActor::SetRootComponent(USceneComponent* InRootComponent)
{
	if (RootComponent == InRootComponent) return;
	//기존 루트 컴포넌트가 있었다면 정리
	if (RootComponent)
	{
		if (OwningScene)
		{
			RootComponent->UnregisterComponentFromScene(*OwningScene); // 씬 렌더 큐에서 제거
		}
		std::erase(AttachedComp, RootComponent); // 액터 참조 목록에서 제거, 이제 GC 대상이 됨
	}
	//새 루트 컴포넌트 장착
	RootComponent = InRootComponent;
	if (RootComponent)
	{
		AttachedComp.push_back(RootComponent);
		//씬에 이미 스폰된 액터라면 새 루트 컴포넌트도 즉시 씬에 등록!
		if (OwningScene)
		{
			RootComponent->RegisterComponentWithScene(*OwningScene);
		}
	}
}

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


	if (OwningScene) //씬이 존재하면 바로 register
	{
		Addcomp->RegisterComponentWithScene(*OwningScene);
	}
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

void AActor::RegisterAllComponents(UScene& Scene)
{
	for (USceneComponent* Comp : AttachedComp)
	{
		if (Comp)
		{
			Comp->RegisterComponentWithScene(Scene);
		}
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
