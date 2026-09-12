#pragma once

#include "Runtime/CoreUObject/UObject.h"
#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include <type_traits>
#include <concepts>

class UScene;

class AActor : public UObject
{
	DECLARE_UCLASS(AActor, UObject)
	GENERATED_BODY()

	friend class UScene;

protected:
	USceneComponent* RootComponent = nullptr;
	TArray<USceneComponent*> AttachedComp;

	explicit AActor() = default;

	virtual void Serialize(FArchive& Archive) const override;
	virtual void Deserialize(const FArchive& Archive) override;

	void SetRootComponent(USceneComponent* InRootComponent); //root 입력받으면서 동시에 AttachedComp에 제일 먼저 넣기

public:
	void Initialize(UObject* Context = nullptr) override;
	void Release() override;
	UScene* GetOwner() const { return Owner; }

	void CreateRootComponent(UClass* ClassType);
	USceneComponent* GetRootComponent() const { return RootComponent; }


	FTransform GetTransform() const { return RootComponent ? RootComponent->GetRelativeTransform() : FTransform{}; }
	void SetTransform(const FTransform& NewTransform) { if (RootComponent) RootComponent->SetRelativeTransform(NewTransform); }

	void AddComponent(USceneComponent* Addcomp);
	virtual void Update(float DeltaTime);

	void AddReferencedObjects(FReferenceCollector& Collector) override;

	void Destroy();

	void RegisterAllComponents(UScene& Scene);
	void UnregisterAllComponents(UScene& Scene);
	void UnregisterComponentFromScene(UScene& Scene);
private:
	UScene* Owner = nullptr; // SpawnActor될 때 설정됨
};
