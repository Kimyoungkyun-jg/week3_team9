#include "UBillBoardComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Engine/UScene.h"
#include "Runtime/Engine/FArchive.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "Runtime/Core/Log.h"
#include "Runtime/Rendering/FRenderer.h"
#include "UClass.h"
#include <algorithm>
#include <cctype>

IMPLEMENT_UCLASS(UBillBoardComp, UPrimitiveComponent)
UCLASS_META(UBillBoardComp, DisplayName, "BillBoard")
UCLASS_META(UBillBoardComp, MeshName, "BillBoard")

void UBillBoardComp::Register(UScene& InScene) {
  FRenderResourceLibrary* Resources = InScene.GetRenderResourceLibrary();
  if (!GetMesh()) {
    SetMesh(Resources ? Resources->GetMesh(EMeshID::Rect) : nullptr);
  }
  if (!GetMaterial()) {
    SetMaterial(Resources ? Resources->GetMaterial(EMaterialID::Textured) : nullptr);
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

    InputConstant.MVP = InputConstant.MVP * View * Proj;
}

void UBillBoardComp::Render(FRenderer &renderer, const FCamera &Camera,

                            const bool &bHighlighted) {
  if (!GetMesh() || !GetMaterial()) {
    return;
  }

  FObjectConstants Constants;

  // 빌보드 회전 계산
  const FTransform G = GetGlobalTransform();
  const FVector ToCamera = Camera.Position - G.Location;

  if (ToCamera.SizeSquared() < 1e-8f) {
    Constants.MVP = FMatrix::GetIdentity();
    Constants.World = FMatrix::GetIdentity();
  } else {
    // 카메라 방향 보정
    const FVector Forward = -ToCamera / ToCamera.Size();
    const FMatrix CamRot = Camera.GetRotationMatrix();
    const FVector CameraUp{CamRot.M[2][0], CamRot.M[2][1], CamRot.M[2][2]};

    FVector Right = CameraUp.Cross(Forward);
    if (Right.SizeSquared() < 1e-8f) {
      Right = FVector{0.0f, 1.0f, 0.0f};
    }
    Right = Right / Right.Size();
    const FVector Up = Forward.Cross(Right);

    const FMatrix Rotation{Forward, Right, Up, FVector{0.0f, 0.0f, 0.0f}};

    const FMatrix World = FMatrix::MakeScale(G.Scale3D) * Rotation *
                          FMatrix::MakeTranslation(G.Location);
    Constants.World = World;

    const FMatrix InvRot = Camera.GetRotationMatrix().Transpose();
    const FMatrix View = FMatrix::MakeTranslation(-Camera.Position) * InvRot;
    const FMatrix Proj = Camera.GetProjectionMatrix();

    Constants.MVP = World * View * Proj;
  }

  // 텍스처 좌표 정보 갱신
  Constants.UVScale = UVScale;
  Constants.UVOffset = UVOffset;

  // 컴포넌트 색상 반영
  Constants.ColorOverride = GetColor();
  Constants.ColorOverrideAmount = GetColorAmount();

  if (bHighlighted) {
    // 하이라이트 색상 보정
    if (Constants.ColorOverrideAmount > 0.0f) {
      Constants.ColorOverride =
          Constants.ColorOverride * 0.7f + FVector{0.3f, 0.3f, 0.3f};
    } else {
      Constants.ColorOverride = FVector{1.0f, 1.0f, 1.0f};
      Constants.ColorOverrideAmount = 0.5f;
    }
  }

  renderer.Draw(*GetMesh(), *GetMaterial(), Constants);
}

void UBillBoardComp::SetTexture(
    FString texture) // 원본 머터리얼을 건드리지 않고 instance로 생성해서 사용
{
  auto &lib = FRenderResourceLibrary::Get();

  // 소문자 변환
  FString LowerName = texture;
  std::transform(LowerName.begin(), LowerName.end(), LowerName.begin(),
                 ::tolower);

  auto NewTex = lib.GetTexture(LowerName);
  if (!NewTex) {
    UE_LOG("There is no such texture");
    return;
  }

  // 머티리얼 인스턴스 생성
  auto materialinstance = TSharedPtr<FMaterial>(new FMaterial());

  if (GetMaterial()) {
    materialinstance->SetPipeLine(GetMaterial()->GetPipeline());
  } else {
    materialinstance->SetPipeLine(lib.GetPipeline(EPipelineID::Textured));
  }

  materialinstance->SetTexture(NewTex);
  SetMaterial(materialinstance);
}
