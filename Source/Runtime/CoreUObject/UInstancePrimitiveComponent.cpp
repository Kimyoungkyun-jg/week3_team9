#include "UInstancePrimitiveComponent.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "Runtime/Rendering/ShaderConstants.h"
#include "Runtime/Engine/UScene.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UInstancePrimitiveComponent, UPrimitiveComponent)

void UInstancePrimitiveComponent::Register(UScene& Scene)
{
	FRenderResourceLibrary* Resources = Scene.GetRenderResourceLibrary();
	if (!PrimitiveMesh)
	{
		SetMesh(Resources ? Resources->GetMesh(FName("Cube")) : nullptr);
	}
	
	if (!PrimitiveMaterial)
	{
		SetMaterial(Resources ? Resources->GetMaterial(FName("Instance_Simple")) : nullptr);
	}

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

void UInstancePrimitiveComponent::BuildRenderData()
{
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

    RenderData.Instances = std::move(Built);
}

const FRenderData& UInstancePrimitiveComponent::GetRenderData(const FCamera& Camera)
{
    BuildRenderData();
    return RenderData;
}
