#include "UClass.h"
#include "USceneComponent.h"
#include "ThirdParty/Json/json.hpp"
#include "UObjectGlobals.h" 


IMPLEMENT_UCLASS(USceneComponent, UObject)

FTransform USceneComponent::GetRelativeTransform()
{
    return RelativeTransform;
}

void USceneComponent::SetRelativeTransform(FTransform RelativeTransform)
{
    this->RelativeTransform = RelativeTransform;
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


