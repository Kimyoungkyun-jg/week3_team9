#pragma once

#include "Runtime/Geometry/FTransform.h"
#include "ThirdParty/Json/json.hpp"
#include "UObject.h"


class UScene;
class AActor;

class USceneComponent : public UObject
{
	GENERATED_BODY()
	DECLARE_UCLASS(USceneComponent, UObject)

public:
  AActor* GetOwner() const { return Owner; }
  void SetOwner(AActor* InOwner) { Owner = InOwner; }

  json::JSON Serialize() const override;
  bool Deserialize(const json::JSON &data) override;
  virtual void Update(float DeltaTime) {}

	virtual void OnRegister(UScene& Scene) {}
	virtual void OnUnregister(UScene& Scene) {}
protected:
  USceneComponent() = default;

	FTransform RelativeTransform;

public:
	FTransform& GetRelativeTransform() { return RelativeTransform; }
	virtual void SetRelativeTransform(FTransform& RelativeTransform);

  void RegisterComponentWithScene(UScene &Scene);
  void UnregisterComponentFromScene(UScene &Scene);

protected:
  AActor* Owner = nullptr; // 소유 액터
};
