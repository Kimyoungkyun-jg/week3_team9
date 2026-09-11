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
  friend UScene;

public:
  AActor* GetOwner() const { return Owner; }
  void SetOwner(AActor* InOwner) { Owner = InOwner; }

  FTransform RelativeTransform;
  json::JSON Serialize() const override;
  bool Deserialize(const json::JSON &data) override;
  virtual void Update(float DeltaTime) {}

protected:
  USceneComponent() = default;

  virtual void OnRegister(UScene &Scene) {}
  virtual void OnUnregister(UScene &Scene) {}

public:
  FTransform GetRelativeTransform();
  void SetRelativeTransform(FTransform RelativeTransform);
  FTransform GetGlobalTransform();
  void SetRelativeTransformFromGlobal(FTransform GlobalTransform);

  void RegisterComponentWithScene(UScene &Scene);
  void UnregisterComponentFromScene(UScene &Scene);

protected:
  AActor* Owner = nullptr; // 소유 액터
};
