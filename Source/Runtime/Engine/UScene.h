#pragma once

#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/CoreUObject/UPrimitiveComponent.h"
#include "Runtime/Core/IntTypes.h"
#include "Runtime/Core/TArray.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"

#include "ThirdParty/Json/json.hpp"

class UScene final : public UObject
{
	GENERATED_BODY()

public:
	void RegisterComponent(USceneComponent& Component);
	void UnregisterComponent(USceneComponent& Component);
	// TODO: 렌더링할 수 있는 컴포넌트만 가져오는데, 다른 방식으로 처리할 수도 있을 듯
	[[nodiscard]] TArray<UPrimitiveComponent*> GetPrimitiveComponents() const;
	// TODO: PrimitiveComponent가 Register할 때 얻으려고 필요한데 다른 방법은 없을까?
	[[nodiscard]] FRenderResourceLibrary& GetRenderResourceLibrary() const { return RenderResourceLibrary; }

	json::JSON Serialize() const;
	virtual bool Deserialize(const json::JSON& data) override;

	void CreateFromJson(json::JSON data);
	
	void AddReferencedObjects(FReferenceCollector& Collector) override;
private:
	explicit UScene(FRenderResourceLibrary& RenderResources)
		: RenderResourceLibrary(RenderResources)
	{}

	

	uint32 Version = 1u;
	uint32 NextUUID = 1u;
	TArray<USceneComponent*> Components{};

	FRenderResourceLibrary& RenderResourceLibrary; // TODO: 추후 엔진으로부터 렌더러 추상화 시 제거
};
