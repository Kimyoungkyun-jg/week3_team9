#pragma once

#include "Runtime/CoreUObject/UObject.h"
#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/CoreUObject/TWeakObjectPtr.h"

class UScene;

class AActor : public UObject
{
	DECLARE_UCLASS(AActor, UObject, RootComponent, AttachedComp)
	GENERATED_BODY()

	friend class UScene;

protected:
	USceneComponent* RootComponent = nullptr;
	TArray<USceneComponent*> AttachedComp;

	explicit AActor() = default;

public:
	USceneComponent* GetRootComponent() const { return RootComponent; }
	const TArray<USceneComponent*>& GetAttachedComponents() const { return AttachedComp; }
	void SetRootComponent(USceneComponent* InRootComponent); //root 입력받으면서 동시에 AttachedComp에 제일 먼저 넣기


	FTransform GetTransform() const { return RootComponent ? RootComponent->GetRelativeTransform() : FTransform{}; }
	void SetTransform(const FTransform& NewTransform) { if (RootComponent) RootComponent->SetRelativeTransform(NewTransform); }

	void AddComponent(USceneComponent* Addcomp);
	virtual void Update(float DeltaTime);

	// 액터 색상 제어
	virtual void SetColor(const FVector& InColor);
	virtual FVector GetColor() const;


	void SetScene(UScene* InScene);
	[[nodiscard]] UScene* GetScene() const;
	void Destroy();

	void RegisterAllComponents(UScene& Scene);
	void UnregisterAllComponents(UScene& Scene);
	void UnregisterComponentFromScene(UScene& Scene);
private:
	TWeakObjectPtr<UScene> OwningScene; // SpawnActor될 때 설정됨
};

