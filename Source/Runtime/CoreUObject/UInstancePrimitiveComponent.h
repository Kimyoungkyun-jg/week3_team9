#pragma once

#include "Runtime/Core/TArray.h"
#include "Runtime/Math/FMatrix.h"
#include "Runtime/Math/FVector2.h"
#include "Runtime/Math/FVector4.h"
#include "UPrimitiveComponent.h"
#include <d3d11.h>
#include <wrl/client.h>

struct FInstanceData {
  FMatrix Word;
  FVector4 Color;
  FVector2 UV;
  FVector2 UVOffset;
};

class UInstancePrimitiveComponent : public UPrimitiveComponent {
  GENERATED_BODY()
  DECLARE_UCLASS(UInstancePrimitiveComponent, UPrimitiveComponent)

public:
  FInstanceData insData;
};
