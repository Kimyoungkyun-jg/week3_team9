#pragma once

#include "Runtime/Core/TArray.h"
#include "Runtime/Rendering/Vertices.h"
#include "UPrimitiveComponent.h"
#include <d3d11.h>
#include <wrl/client.h>

class UInstancePrimitiveComponent : public UPrimitiveComponent {
  GENERATED_BODY()
  DECLARE_UCLASS(UInstancePrimitiveComponent, UPrimitiveComponent)


public:
	TArray<FInstanceData> Instances;
};
