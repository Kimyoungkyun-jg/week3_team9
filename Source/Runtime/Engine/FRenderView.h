#pragma once

#include "Runtime/CoreUObject/UPrimitiveComponent.h"
#include "Runtime/Geometry/FTransform.h"
#include "Runtime/Math/FVector2.h"
#include "Runtime/Rendering/FRenderer.h"

struct FCamera;
class FGizmo;
class FGrid;

class FRenderView final {
	FRenderer& Renderer;

public:
	FRenderView(FRenderer& Renderer);
	FRenderer& GetRenderer() { return Renderer; }
	const FRenderer& GetRenderer() const { return Renderer; }

	void Render(const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, UPrimitiveComponent* Rendered, bool bHighlighted = false);
	void RenderGizmo(const FTransform& Transform, const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, const FGizmo& Gizmo);
	void RenderGrid(const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, FGrid& Grid);
};
