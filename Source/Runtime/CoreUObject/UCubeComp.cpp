#include "UCubeComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UCubeComp, UPrimitiveComponent)
UCLASS_META(UCubeComp, DisplayName, "Cube")
UCLASS_META(UCubeComp, MeshName, "Cube")

void UCubeComp::OnRegister(UScene& Scene)
{
	UPrimitiveComponent::OnRegister(Scene);

	auto& ResLib = FRenderResourceLibrary::Get();
	SetMesh(ResLib.GetMesh("Cube"));
	SetMaterial(ResLib.GetMaterial("Textured"));
}
