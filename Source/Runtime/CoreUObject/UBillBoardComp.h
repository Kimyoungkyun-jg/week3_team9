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

    FMatrix GetRenderMatrix(const FCamera& Camera) const override
    {
        static const FVector WorldUp{ 0.0f, 0.0f, 1.0f };   // Z-up 기준

        const FVector ToCamera = Camera.Position - RelativeTransform.Location;

        // 카메라가 빌보드 바로 위/아래면 외적이 영벡터가 되어 터진다
        FVector Forward = ToCamera / ToCamera.Size();
        FVector Right = WorldUp.Cross(Forward);
        if (Right.SizeSquared() < 1e-6f) {
            Right = FVector{ 0.0f, 1.0f, 0.0f };   // 대체 축
        }
        Right = Right / Right.Size();

        const FVector Up = Forward.Cross(Right);   // 이미 단위벡터 (직교하는 단위벡터의 외적)

        // 행 = "로컬 축이 월드의 어디로 가는가"
        // 쿼드가 YZ 평면(로컬 X가 법선)이면: X→Forward, Y→Right, Z→Up
        const FMatrix Rotation{ Right, Up, Forward, FVector{ 0.0f, 0.0f, 0.0f } };

        return FMatrix::MakeScale(RelativeTransform.Scale3D)
            * Rotation
            * FMatrix::MakeTranslation(RelativeTransform.Location);
    }
	// 빌보드 회전 계산
	void CalculateRotate(const FCamera& Camera, FObjectConstants& InputConstant);

    virtual void UpdateUVinfo(FObjectConstants& InputConstant) {};

private:
	// 시선 회전 보간용 쿼터니언
	FQuaternion CurrentRotation = FQuaternion::Identity();
    
    FVector2 UVScale{ 1.0f, 1.0f };
    FVector2 UVOffset{ 0.0f, 0.0f };

};