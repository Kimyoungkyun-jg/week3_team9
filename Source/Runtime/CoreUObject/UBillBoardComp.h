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

	virtual void UpdateUVinfo(FObjectConstants &InputConstant) {};
	virtual void SetTexture(FString texture);
private:
  // 시선 회전 보간용 쿼터니언
  FQuaternion CurrentRotation = FQuaternion::Identity();

  FVector2 UVScale{1.0f, 1.0f};
  FVector2 UVOffset{0.0f, 0.0f};
};