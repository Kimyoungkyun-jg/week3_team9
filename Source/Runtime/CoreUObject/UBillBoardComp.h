#pragma once
#include "Runtime/Engine/FCamera.h"
#include "Runtime/Math/FQuaternion.h"
#include "Runtime/Rendering/ShaderConstants.h"
#include "UPrimitiveComponent.h"

class UScene;
class FArchive;

class UBillBoardComp : public UPrimitiveComponent {
  DECLARE_UCLASS(UBillBoardComp, UPrimitiveComponent)
  GENERATED_BODY()

protected:
  explicit UBillBoardComp() = default;

  virtual void Serialize(FArchive& Archive) const;
  virtual void Deserialize(const FArchive& Archive);

public:
	void Register(UScene& InScene) override;
	// 빌보드 회전 계산
	void CalculateRotate(const FCamera& Camera, FObjectConstants& InputConstant);

	virtual void UpdateUVinfo(FObjectConstants &InputConstant) {};
	virtual void SetTexture(FString texture); //원본 머터리얼을 건드리지 않고 instance로 생성해서 사용
private:
  // 시선 회전 보간용 쿼터니언
  FQuaternion CurrentRotation = FQuaternion::Identity();

  FVector2 UVScale{1.0f, 1.0f};
  FVector2 UVOffset{0.0f, 0.0f};
};
