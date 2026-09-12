#include "USpotLightComponent.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "UClass.h"

IMPLEMENT_UCLASS(USpotLightComponent, UPrimitiveComponent)
UCLASS_META(USpotLightComponent, DisplayName, "SpotLight")
UCLASS_META(USpotLightComponent, MeshName, "SpotlightCone")

void USpotLightComponent::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	// 원뿔 메시 및 스포트라이트 머티리얼 장착
	auto& ResLib = FRenderResourceLibrary::Get();
	SetMesh(ResLib.GetMesh("SpotlightCone"));
	SetMaterial(ResLib.GetMaterial("Spotlight"));
}
