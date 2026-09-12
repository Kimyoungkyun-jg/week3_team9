#include "UScene.h"

#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/CoreUObject/UPrimitiveComponent.h"
#include "Runtime/Core/TArray.h"
#include "Runtime/Core/FString.h"
#include "Runtime/CoreUObject/UClass.h"
#include "Runtime/CoreUObject/FReferenceCollector.h"
#include "Runtime/Engine/FArchive.h"
#include <algorithm>

IMPLEMENT_UCLASS(UScene, UObject)
UCLASS_META(UScene, SerializeName, "Scene")

TArray<UPrimitiveComponent*> UScene::GetRenderComponents() const
{
	return RenderComponents;
}

void UScene::SetRenderResourceLibrary(FRenderResourceLibrary* InRenderResourceLibrary)
{
	RenderResourceLibrary = InRenderResourceLibrary;
}

void UScene::Serialize(FArchive& Archive) const
{
	Super::Serialize(Archive);

	TArray<FArchive> ActorArchives;

	for (const auto& Item : Actors)
	{
		if (!Item) { continue; }

		FArchive ItemArchive;
		Item->Serialize(ItemArchive);
		ActorArchives.push_back(ItemArchive);
	}

	Archive.SetArchiveArray("Actors", ActorArchives);
}

void UScene::Deserialize(const FArchive& Archive)
{
	Super::Deserialize(Archive);

	if (Archive.IsNull("Actors"))
	{
		// Actor 목록이 비어있음
		return;
	}

	TArray<FArchive> ActorArchives = Archive.GetArchiveArray("Actors");

	for (const auto& Item : ActorArchives)
	{
		UClass* ClassType = UClass::FindByName(Item.GetString("Type"));
		AActor* Actor = SpawnActor(ClassType);
		Actor->Deserialize(Item);
	}
}

void UScene::AddReferencedObjects(FReferenceCollector& Collector)
{
	UObject::AddReferencedObjects(Collector);

	// 액터 참조 수집
	for (AActor* Actor : Actors)
		Collector.AddReferencedObject(Actor);

	for (USceneComponent* Component : RenderComponents)
		Collector.AddReferencedObject(Component);
}

void UScene::AddRenderComponent(UPrimitiveComponent* prim)
{
	if (prim == nullptr) return;

	if (std::find(RenderComponents.begin(), RenderComponents.end(), prim) == RenderComponents.end())
	{
		RenderComponents.push_back(prim);
	}
}

void UScene::RemoveRenderComponent(UPrimitiveComponent* prim)
{
	std::erase(RenderComponents, prim);
}

AActor* UScene::SpawnActor(UClass* ClassType)
{
	AActor* Actor = NewObject(ClassType)->Cast<AActor>();

	Actor->Initialize(this);
	Actor->RegisterAllComponents(*this); // 스폰될때 attached 에 들어가 있는애들 바로 다 등록

	Actors.push_back(Actor);
	return Actor;
}

