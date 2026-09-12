#include "UClass.h"
#include "USceneComponent.h"
#include "ThirdParty/Json/json.hpp"
#include "UObjectGlobals.h" 
#include "UPrimitiveComponent.h"
#include "Runtime/Actors/AActor.h"
#include "../Engine/UScene.h"


IMPLEMENT_UCLASS(USceneComponent, UObject)

AActor* USceneComponent::GetOwner() const
{
    return Owner.Get();
}

void USceneComponent::SetOwner(AActor* InOwner)
{
    Owner = InOwner;
}

void USceneComponent::SetRelativeTransform(const FTransform& RelativeTransform)
{
    this->RelativeTransform = RelativeTransform;
}

FTransform USceneComponent::GetGlobalTransform() const
{
    if (!Owner || Owner->GetRootComponent() == this)
    {
        return RelativeTransform;
    }

    // 부모 트랜스폼과 상대 트랜스폼 합성
    FTransform ParentWorld = Owner->GetRootComponent()->GetGlobalTransform();
    return ParentWorld * RelativeTransform;
}

json::JSON USceneComponent::Serialize() const
{
    FVector tmpRot = RelativeTransform.Rotation.GetEulerXYZ();
    json::JSON result;
    result["Location"] = json::Array(RelativeTransform.Location.X, RelativeTransform.Location.Y, RelativeTransform.Location.Z);
    result["Rotation"] = json::Array(tmpRot.X, tmpRot.Y, tmpRot.Z);
    result["Scale"] = json::Array(RelativeTransform.Scale3D.X, RelativeTransform.Scale3D.Y, RelativeTransform.Scale3D.Z);
    result["Type"] = GetClass()->GetDisplayName();
    return result;
}

bool USceneComponent::Deserialize(const json::JSON& data)
{
    auto location = data.at("Location");
    if (location.size() != 3) return false;
    RelativeTransform.Location = FVector(location[0].ToFloat(), location[1].ToFloat(), location[2].ToFloat());

    auto rotation = data.at("Rotation");
    if (rotation.size() != 3) return false;
    constexpr float RadToDeg = 180.0f / std::numbers::pi_v<float>;

    RelativeTransform.Rotation = FQuaternion::FromEulerXYZDeg({ static_cast<float>(rotation[0].ToFloat()) * RadToDeg, static_cast<float>(rotation[1].ToFloat()) * RadToDeg, static_cast<float>(rotation[2].ToFloat()) * RadToDeg });

    auto scale = data.at("Scale");
    if (scale.size() != 3) return false;
    RelativeTransform.Scale3D = FVector(scale[0].ToFloat(), scale[1].ToFloat(), scale[2].ToFloat());


    return true;
}


void USceneComponent::RegisterComponentWithScene(UScene& Scene)
{
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
}