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

FObjectConstants UBillBoardComp::CalculateRotate(const FCamera& Camera, uint32 signal)
{
  FObjectConstants Result;
  const FTransform GlobalTransform = GetGlobalTransform();
  const FVector BillboardPos = GlobalTransform.Location;
  const FVector Scale = GlobalTransform.Scale3D;

  if (signal == 0) {
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

    Result.MVP = ModelView * Proj;
  } else {
    // 목표 시선 각도 계산
    FVector Dir = Camera.Position - BillboardPos;
    float DistH = std::sqrt(Dir.X * Dir.X + Dir.Y * Dir.Y);

    constexpr float RadToDeg = 180.0f / std::numbers::pi_v<float>;
    float TargetYaw = std::atan2(Dir.Y, Dir.X) * RadToDeg;
    float TargetPitch = std::atan2(Dir.Z, DistH) * RadToDeg;

    // 목표 쿼터니언 생성
    FQuaternion TargetQuat = FQuaternion::FromEulerXYZDeg(FVector(0.0f, TargetPitch, TargetYaw));

    // 쿼터니언 보간 및 정규화
    constexpr float Alpha = 0.1f;
    CurrentRotation.X = CurrentRotation.X * 0.9f + TargetQuat.X * Alpha;
    CurrentRotation.Y = CurrentRotation.Y * 0.9f + TargetQuat.Y * Alpha;
    CurrentRotation.Z = CurrentRotation.Z * 0.9f + TargetQuat.Z * Alpha;
    CurrentRotation.W = CurrentRotation.W * 0.9f + TargetQuat.W * Alpha;
    CurrentRotation.Normalize();

    // 회전 행렬 생성
    FMatrix RotMat = CurrentRotation.ToMatrixRow();

    // 최종 모델 변환 행렬 구성
    FMatrix ScaleMat = FMatrix::MakeScale(Scale);
    FMatrix TransMat = FMatrix::MakeTranslation(BillboardPos);
    FMatrix Model = ScaleMat * RotMat * TransMat;

    Result.MVP = Model * Camera.CreateViewProjectionMatrix();
  }

  return Result;
}
