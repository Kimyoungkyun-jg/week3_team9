#include "UInstancePrimitiveComponent.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "Runtime/Rendering/ShaderConstants.h"
#include "Runtime/Engine/UScene.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UInstancePrimitiveComponent, UPrimitiveComponent)

void UInstancePrimitiveComponent::Register(UScene& Scene)
{
    SetMeshID(EMeshID::Cube);
    SetMaterialID(EMaterialID::Instance_Simple);
    Super::Register(Scene);
}

void UInstancePrimitiveComponent::AddInstance(const FVector& WorldPosition, const FVector4& Color)
{
    InstanceTransforms.push_back({ WorldPosition, Color });
}

void UInstancePrimitiveComponent::ClearInstances()
{
    InstanceTransforms.clear();
}

FRenderData UInstancePrimitiveComponent::BuildRenderData() const
{
    FRenderData Data;
    Data.type       = ERenderType::Instancing;
    Data.MeshId     = RenderData.MeshId;
    Data.MaterialId = RenderData.MaterialId;
    Data.flag       = GetShowFlag();

    TArray<FInstanceData> Built;

    if (InstanceTransforms.empty())
    {
        // 등록된 인스턴스 없으면 자기 자신 트랜스폼 1개
        Built.push_back(FInstanceData{
            .World    = GetGlobalTransform().ToMatrix(),
            .Color    = FVector4(GetColor(), 1.0f),
            .UVScale  = {1.0f, 1.0f},
            .UVOffset = {0.0f, 0.0f},
        });
    }
    else
    {
        Built.reserve(InstanceTransforms.size());
        for (const auto& Entry : InstanceTransforms)
        {
            Built.push_back(FInstanceData{
                .World    = FMatrix::MakeTranslation(Entry.Position),
                .Color    = Entry.Color,
                .UVScale  = {1.0f, 1.0f},
                .UVOffset = {0.0f, 0.0f},
            });
        }
    }

    Data.Instances = std::move(Built);
    return Data;
}
