#include "UBillBoardComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Engine/UScene.h"
#include "Runtime/Engine/FArchive.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "Runtime/Core/Log.h"
#include "UClass.h"
#include <algorithm>
#include <cctype>

IMPLEMENT_UCLASS(UBillBoardComp, UPrimitiveComponent)
UCLASS_META(UBillBoardComp, DisplayName, "BillBoard")
UCLASS_META(UBillBoardComp, MeshName, "BillBoard")

void UBillBoardComp::Register(UScene& InScene) {
  FRenderResourceLibrary* Resources = InScene.GetRenderResourceLibrary();
  if (!GetMesh()) {
    SetMesh(Resources ? Resources->GetMesh("Rect") : nullptr);
  }
  if (!GetMaterial()) {
    SetMaterial(Resources ? Resources->GetMaterial("Textured") : nullptr);
  }
  Super::Register(InScene);
}

void UBillBoardComp::Serialize(FArchive& Archive) const
{
    Super::Serialize(Archive);

    Archive.SetVector2("UVScale", UVScale);
    Archive.SetVector2("UVOffset", UVOffset);
}

void UBillBoardComp::Deserialize(const FArchive& Archive)
{
    Super::Deserialize(Archive);

    UVScale = Archive.GetVector2("UVScale");
    UVOffset = Archive.GetVector2("UVOffset");
}

void UBillBoardComp::CalculateRotate(const FCamera& Camera, FObjectConstants& InputConstant)
{
    const FTransform G = GetGlobalTransform();
    const FVector ToCamera = Camera.Position - G.Location;


    if (ToCamera.SizeSquared() < 1e-8f) {
        InputConstant.MVP = FMatrix::GetIdentity();
    }
    const FVector Forward = ToCamera / ToCamera.Size();
    const FMatrix CamRot = Camera.GetRotationMatrix();
    const FVector CameraUp{ CamRot.M[2][0], CamRot.M[2][1], CamRot.M[2][2] };

    FVector Right = CameraUp.Cross(Forward);
    if (Right.SizeSquared() < 1e-8f) {
        Right = FVector{ 0.0f, 1.0f, 0.0f };
    }
    Right = Right / Right.Size();
    const FVector Up = Forward.Cross(Right);

    const FMatrix Rotation{ Forward, Right, Up, FVector{ 0.0f, 0.0f, 0.0f } };

    InputConstant.MVP = FMatrix::MakeScale(G.Scale3D)
        * Rotation
        * FMatrix::MakeTranslation(G.Location);

    const FMatrix InvRot = Camera.GetRotationMatrix().Transpose();
    const FMatrix View = FMatrix::MakeTranslation(-Camera.Position) * InvRot;
    const FMatrix Proj = Camera.GetProjectionMatrix();

    InputConstant.MVP = InputConstant.MVP * View* Proj;
}

void UBillBoardComp::SetTexture(FString Texture)
{
    std::transform(Texture.begin(), Texture.end(), Texture.begin(),
        [](unsigned char Character) { return static_cast<char>(std::tolower(Character)); });

    auto& Resources = FRenderResourceLibrary::Get();
    auto NewTexture = Resources.GetTexture(Texture);
    if (!NewTexture)
    {
        UE_LOG("There is no such texture");
        return;
    }

    auto MaterialInstance = TSharedPtr<FMaterial>(new FMaterial());
    if (GetMaterial())
    {
        MaterialInstance->SetPipeLine(GetMaterial()->GetPipeline());
    }
    else
    {
        MaterialInstance->SetPipeLine(Resources.GetPipeline(EBuiltinPipeline::Textured));
    }
    MaterialInstance->SetTexture(NewTexture);
    SetMaterial(MaterialInstance);
}
