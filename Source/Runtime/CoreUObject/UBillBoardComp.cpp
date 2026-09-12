#include "UBillBoardComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Engine/UScene.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UBillBoardComp, UPrimitiveComponent)
UCLASS_META(UBillBoardComp, DisplayName, "BillBoard")
UCLASS_META(UBillBoardComp, MeshName, "BillBoard")

void UBillBoardComp::OnRegister(UScene &Scene) {
  UPrimitiveComponent::OnRegister(Scene);

  auto &ResLib = FRenderResourceLibrary::Get();
  if (!GetMesh()) //이미 있으면 다시 등록x
  {
    SetMesh(ResLib.GetMesh("Rect"));
  }
  if (!GetMaterial()) //이미 있으면 다시 등록x
  {
    SetMaterial(ResLib.GetMaterial("Textured"));
  }
}

void UBillBoardComp::CalculateRotate(const FCamera &Camera,
                                     FObjectConstants &InputConstant) {
  const FTransform G = GetGlobalTransform();
  const FVector ToCamera = Camera.Position - G.Location;

  if (ToCamera.SizeSquared() < 1e-8f) {
    InputConstant.MVP = FMatrix::GetIdentity();
    return;
  }
  const FVector Forward = ToCamera / ToCamera.Size();
  const FMatrix CamRot = Camera.GetRotationMatrix();
  const FVector CameraUp{CamRot.M[2][0], CamRot.M[2][1], CamRot.M[2][2]};

  FVector Right = CameraUp.Cross(Forward);
  if (Right.SizeSquared() < 1e-8f) {
    Right = FVector{0.0f, 1.0f, 0.0f};
  }
  Right = Right / Right.Size();
  const FVector Up = Forward.Cross(Right);

  const FMatrix Rotation{Forward, Right, Up, FVector{0.0f, 0.0f, 0.0f}};

  InputConstant.MVP = FMatrix::MakeScale(G.Scale3D) * Rotation *
                      FMatrix::MakeTranslation(G.Location);

  const FMatrix InvRot = Camera.GetRotationMatrix().Transpose();
  const FMatrix View = FMatrix::MakeTranslation(-Camera.Position) * InvRot;
  const FMatrix Proj = Camera.GetProjectionMatrix();

  InputConstant.MVP = InputConstant.MVP * View * Proj;
}

void UBillBoardComp::SetTexture(FString texture) //원본 머터리얼을 건드리지 않고 instance로 생성해서 사용
{
    auto& lib = FRenderResourceLibrary::Get();
    
    // 소문자 변환
    FString LowerName = texture;
    std::transform(LowerName.begin(), LowerName.end(), LowerName.begin(), ::tolower);

    auto NewTex = lib.GetTexture(LowerName);
    if (!NewTex)
    {
        UE_LOG("There is no such texture");
        return;
    }
    
    // 머티리얼 인스턴스 생성
    auto materialinstance = TSharedPtr<FMaterial>(new FMaterial());
    
    if (GetMaterial())
    {
        materialinstance->SetPipeLine(GetMaterial()->GetPipeline());
    }
    else
    {
        materialinstance->SetPipeLine(lib.GetPipeline(EBuiltinPipeline::Textured));
    }
   
    materialinstance->SetTexture(NewTex);
    SetMaterial(materialinstance);
}
