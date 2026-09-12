#pragma once
#include "Runtime/Engine/FCamera.h"
#include "Runtime/Math/FQuaternion.h"
#include "Runtime/Rendering/ShaderConstants.h"
#include "UPrimitiveComponent.h"

class UScene;
class UBillBoardComp : public UPrimitiveComponent {
  DECLARE_UCLASS(UBillBoardComp, UPrimitiveComponent)
  GENERATED_BODY()

protected:
  explicit UBillBoardComp() = default;

public:
	void OnRegister(UScene& Scene) override;
	// 빌보드 회전 계산
	void CalculateRotate(const FCamera& Camera, FObjectConstants& InputConstant);

    const FVector Up =
        Forward.Cross(Right); // 이미 단위벡터 (직교하는 단위벡터의 외적)

    // 행 = "로컬 축이 월드의 어디로 가는가"
    // 쿼드가 YZ 평면(로컬 X가 법선)이면: X→Forward, Y→Right, Z→Up
    const FMatrix Rotation{Right, Up, Forward, FVector{0.0f, 0.0f, 0.0f}};

    return FMatrix::MakeScale(RelativeTransform.Scale3D) * Rotation *
           FMatrix::MakeTranslation(RelativeTransform.Location);
  }
  // 빌보드 회전 계산
  void CalculateRotate(const FCamera &Camera, FObjectConstants &InputConstant);

  virtual void UpdateUVinfo(FObjectConstants &InputConstant) {};

  // 텍스처 이름으로 머티리얼 텍스처 교체
  bool SetTextureByName(const FString &InTextureName);
  const FString &GetTextureName() const { return TextureName; }

protected:
  UScene *OwningScene = nullptr;
  FString TextureName;

private:
  // 시선 회전 보간용 쿼터니언
  FQuaternion CurrentRotation = FQuaternion::Identity();

  FVector2 UVScale{1.0f, 1.0f};
  FVector2 UVOffset{0.0f, 0.0f};
};