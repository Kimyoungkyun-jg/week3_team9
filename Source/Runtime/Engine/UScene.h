#pragma once

#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/CoreUObject/UPrimitiveComponent.h"
#include "Runtime/Core/IntTypes.h"
#include "Runtime/Core/TArray.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Actors/AActor.h"
#include <concepts>
#include <type_traits>

#include "ThirdParty/Json/json.hpp"

class UScene final : public UObject
{
	GENERATED_BODY()

public:
	void RegisterComponent(USceneComponent& Component);
	void UnregisterComponent(USceneComponent& Component);
	// 렌더링 컴포넌트 목록 반환
	[[nodiscard]] TArray<UPrimitiveComponent*> GetPrimitiveComponents() const;
	[[nodiscard]] FRenderResourceLibrary& GetRenderResourceLibrary() const { return RenderResourceLibrary; }

	// 액터 목록 반환
	[[nodiscard]] const TArray<AActor*>& GetActors() const { return Actors; }

	// 위치와 크기를 지정하여 액터 생성
	template <typename TActor, typename... TArgs>
		requires std::derived_from<TActor, AActor>
	TActor* SpawnActor(const FVector& Location, const FVector& Scale, TArgs&&... Args)
	{
		TActor* Actor = NewObject<TActor>(std::forward<TArgs>(Args)...);
		Actor->SetRootComponent(NewObject<UPrimitiveComponent>());
		if (Actor->GetRootComponent())
		{
			FTransform Transform{};
			Transform.Location = Location;
			Transform.Scale3D = Scale;
			Actor->GetRootComponent()->SetRelativeTransform(Transform);
			//RegisterComponent(*Actor->GetRootComponent());
		}
		Actors.push_back(Actor);
		return Actor;
	}

	// 기본 위치와 크기로 액터 생성
	template <typename TActor>
		requires std::derived_from<TActor, AActor>
	TActor* SpawnActor()
	{
		return SpawnActor<TActor>(FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f));
	}

	// 첫번째 인자가 벡터가 아닐 때 기본 위치와 크기 전달
	template <typename TActor, typename FirstArg, typename... RestArgs>
		requires std::derived_from<TActor, AActor> && (!std::is_same_v<std::decay_t<FirstArg>, FVector>)
	TActor* SpawnActor(FirstArg&& First, RestArgs&&... Rest)
	{
		return SpawnActor<TActor>(FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f),
			std::forward<FirstArg>(First), std::forward<RestArgs>(Rest)...);
	}

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
	TArray<AActor*> Actors{};

	FRenderResourceLibrary& RenderResourceLibrary;
};
