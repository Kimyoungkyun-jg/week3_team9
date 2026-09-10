#pragma once

#include "UObject.h"
#include "Runtime/Geometry/FTransform.h"
#include "ThirdParty/Json/json.hpp"

class UScene;

class USceneComponent : public UObject
{
	GENERATED_BODY()
	DECLARE_UCLASS(USceneComponent, UObject)
	friend UScene;

public:
	FTransform RelativeTransform;
	json::JSON Serialize() const override;
	bool Deserialize(const json::JSON& data) override;
protected:
	USceneComponent() = default;

	virtual void OnRegister(UScene& Scene) {}
	virtual void OnUnregister(UScene& Scene) {}
public:
	FTransform GetRelativeTransform();
	void SetRelativeTransform(FTransform RelativeTransform);
	FTransform GetGlobalTransform();
	void SetRelativeTransformFromGlobal(FTransform GlobalTransform);
};
