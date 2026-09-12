#pragma once

#include "Runtime/Geometry/FTransform.h"
#include "ThirdParty/Json/json.hpp"
#include "UObject.h"


class UScene;
class AActor;
class FArchive;

class USceneComponent : public UObject
{
	GENERATED_BODY()
	DECLARE_UCLASS(USceneComponent, UObject)

public:
    virtual void Initialize(UObject* Context = nullptr) override;
    virtual void Release() override;
    
    AActor* GetActorOwner() const { return ActorOwner; }
    USceneComponent* GetSceneOwner() const { return SceneOwner; }

    virtual void Update(float DeltaTime) {}

	virtual void Serialize(FArchive& Archive) const override;
	virtual void Deserialize(const FArchive& Archive) override;

protected:
	USceneComponent() = default;

	FTransform RelativeTransform;

public:
	FTransform& GetRelativeTransform() { return RelativeTransform; }
	virtual void SetRelativeTransform(const FTransform& RelativeTransform);
	FTransform GetGlobalTransform();
	//void SetRelativeTransformFromGlobal(const FTransform& GlobalTransform);

  void RegisterComponentWithScene(UScene &Scene);
  void UnregisterComponentFromScene(UScene &Scene);

protected:
  AActor* ActorOwner = nullptr;
  USceneComponent* SceneOwner = nullptr;
  UScene* Scene = nullptr;
};
