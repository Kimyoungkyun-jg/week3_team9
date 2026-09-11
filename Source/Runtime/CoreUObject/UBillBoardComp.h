#pragma once
#include "UPrimitiveComponent.h"
class UScene;
class UBillBoardComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UBillBoardComp, UPrimitiveComponent)
	GENERATED_BODY()

protected:
	explicit UBillBoardComp() = default;

public:
	void OnRegister(UScene& Scene) override;

};