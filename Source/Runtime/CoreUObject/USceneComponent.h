#pragma once

#include "UObject.h"
#include "Runtime/Geometry/FTransform.h"
#include "ThirdParty/Json/json.hpp"

class UScene;

class USceneComponent : public UObject //
{
	GENERATED_BODY()
	DECLARE_UCLASS(USceneComponent, UObject)

public:

	json::JSON Serialize() const override;
	bool Deserialize(const json::JSON& data) override;
	virtual void Update(float DeltaTime) {}


	virtual void OnRegister(UScene& Scene) {}
	virtual void OnUnregister(UScene& Scene) {}

protected:
	USceneComponent() = default;

	FTransform RelativeTransform;

public:
	FTransform& GetRelativeTransform() { return RelativeTransform; }
	virtual void SetRelativeTransform(FTransform& RelativeTransform);
	void RegisterComponentWithScene(UScene& Scene);
	void UnregisterComponentFromScene(UScene& Scene);
};
