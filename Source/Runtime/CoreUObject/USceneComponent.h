#pragma once

#include "Runtime/Geometry/FTransform.h"
#include "ThirdParty/Json/json.hpp"
#include "UObject.h"
#include "TWeakObjectPtr.h"


class UScene;
class AActor;

class USceneComponent : public UObject
{
	GENERATED_BODY()
	DECLARE_UCLASS(USceneComponent, UObject)

public:
  AActor* GetOwner() const;
  void SetOwner(AActor* InOwner);

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
	const FTransform& GetRelativeTransform() const { return RelativeTransform; }
	virtual void SetRelativeTransform(const FTransform& RelativeTransform);
	FTransform GetGlobalTransform() const;
	//void SetRelativeTransformFromGlobal(const FTransform& GlobalTransform);

  void RegisterComponentWithScene(UScene &Scene);
  void UnregisterComponentFromScene(UScene &Scene);

protected:
  TWeakObjectPtr<AActor> Owner;
};
