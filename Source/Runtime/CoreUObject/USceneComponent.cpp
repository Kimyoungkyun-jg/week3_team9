#include "UClass.h"
#include "USceneComponent.h"
#include "ThirdParty/Json/json.hpp"
#include "UObjectGlobals.h" 
#include "UPrimitiveComponent.h"
#include "Runtime/Engine/FArchive.h"
#include "Runtime/Engine/UScene.h"


IMPLEMENT_UCLASS(USceneComponent, UObject)

void USceneComponent::Initialize(UObject* Context)
{
    Super::Initialize(Context);

    ActorOwner = nullptr;
    SceneOwner = nullptr;
    Scene = nullptr;

    if (Context == nullptr) { return; }

    if (AActor* Actor = Context->Cast<AActor>())
    {
        ActorOwner = Actor;
        Scene = Actor->GetOwner();
        return;
    }

    USceneComponent* Component = Context->Cast<USceneComponent>();
    if (Component && Component != this)
    {
        SceneOwner = Component;
        ActorOwner = Component->GetActorOwner();
        Scene = ActorOwner->GetOwner();
    }
}

void USceneComponent::Serialize(FArchive& Archive) const
{
    Super::Serialize(Archive);

    Archive.SetVector("Location", RelativeTransform.Location);
    Archive.SetVector("Rotation", RelativeTransform.Rotation.GetEulerXYZ());
    Archive.SetVector("Scale", RelativeTransform.Scale3D);
}

void USceneComponent::Deserialize(const FArchive& Archive)
{
    Super::Deserialize(Archive);

    // Location
    RelativeTransform.Location = Archive.GetVector("Location");

    // Rotation
    constexpr float RadToDeg = 180.0f / std::numbers::pi_v<float>;
    FVector Rotation = Archive.GetVector("Rotation");
    for (int i = 0; i < 3; ++i)
    {
        Rotation[i] *= RadToDeg;
    }
    RelativeTransform.Rotation.FromEulerXYZDeg(Rotation);

    // Scale
    RelativeTransform.Scale3D = Archive.GetVector("Scale");
}

void USceneComponent::SetRelativeTransform(const FTransform& RelativeTransform)
{
    this->RelativeTransform = RelativeTransform;
}

FTransform USceneComponent::GetGlobalTransform() //나중에 부모 rootcomponent world좌표 써야됨
{
    if (SceneOwner)
    {
        return SceneOwner->GetGlobalTransform() * RelativeTransform;
    }

    if (!ActorOwner || ActorOwner->GetRootComponent() == this)
    {
        return RelativeTransform;
    }

    //부모(RootComponent)의 월드 트랜스폼 가져오기
    FTransform ParentWorld = ActorOwner->GetRootComponent()->GetGlobalTransform();
    //부모 트랜스폼 * 내 상대 트랜스폼
    return ParentWorld * RelativeTransform;
}

void USceneComponent::RegisterComponentWithScene(UScene& Scene)
{
	this->Scene = &Scene;

    //가상함수 호출 (자식 컴포넌트가 메시/머티리얼 바인딩)
    OnRegister(Scene);
    //자신이 그릴 수 있는 프리미티브라면 씬의 렌더 큐에 자신을 등록
    if (auto* Prim = this->Cast<UPrimitiveComponent>())
    {
        Scene.AddRenderComponent(Prim);
    }
}

void USceneComponent::UnregisterComponentFromScene(UScene& Scene)
{
    if (auto* Prim = this->Cast<UPrimitiveComponent>())
    {
        Scene.RemoveRenderComponent(Prim);
    }
    OnUnregister(Scene);

	if (this->Scene == &Scene)
	{
		this->Scene = nullptr;
	}
}
