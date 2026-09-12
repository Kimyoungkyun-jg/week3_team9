#include "UBillBoardComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Engine/UScene.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UBillBoardComp, UPrimitiveComponent)
UCLASS_META(UBillBoardComp, DisplayName, "BillBoard")
UCLASS_META(UBillBoardComp, MeshName, "BillBoard")

void UBillBoardComp::OnRegister(UScene &Scene) {
  UPrimitiveComponent::OnRegister(Scene);

  SetMesh(Scene.GetRenderResourceLibrary().GetMesh("Rect"));
  SetMaterial(Scene.GetRenderResourceLibrary().GetMaterial("Textured"));
}

void UBillBoardComp::CalculateRotate(const FCamera &Camera,
                                     FObjectConstants &InputConstant) {
  const FTransform GlobalTransform = GetGlobalTransform();
  const FVector BillboardPos = GlobalTransform.Location;
  const FVector Scale = GlobalTransform.Scale3D;

  // 뷰 공간 기준 회전 정렬
  const FMatrix InvRot = Camera.GetRotationMatrix().Transpose();
  const FMatrix View = FMatrix::MakeTranslation(-Camera.Position) * InvRot;
  const FMatrix Proj = Camera.GetProjectionMatrix();

  FMatrix ModelView = FMatrix::MakeTranslation(BillboardPos) * View;

  // 화면 축 매핑
  ModelView.M[0][0] = Scale.X;
  ModelView.M[0][1] = 0.0f;
  ModelView.M[0][2] = 0.0f;
  ModelView.M[1][0] = 0.0f;
  ModelView.M[1][1] = Scale.Y;
  ModelView.M[1][2] = 0.0f;
  ModelView.M[2][0] = 0.0f;
  ModelView.M[2][1] = 0.0f;
  ModelView.M[2][2] = Scale.Z;

  InputConstant.MVP = ModelView * Proj;
}
