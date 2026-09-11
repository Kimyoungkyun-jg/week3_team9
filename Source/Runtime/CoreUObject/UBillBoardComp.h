#pragma once
#include "UPrimitiveComponent.h"
#include "Runtime/Engine/FCamera.h"
#include "Runtime/Math/FQuaternion.h"
#include "Runtime/Rendering/ShaderConstants.h"

class UScene;
class UBillBoardComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UBillBoardComp, UPrimitiveComponent)
	GENERATED_BODY()

protected:
	explicit UBillBoardComp() = default;

public:
	void OnRegister(UScene& Scene) override;

	// 빌보드 회전 계산
	FObjectConstants CalculateRotate(const FCamera& Camera, uint32 Signal = 0);

private:
	// 시선 회전 보간용 쿼터니언
	FQuaternion CurrentRotation = FQuaternion::Identity();
};