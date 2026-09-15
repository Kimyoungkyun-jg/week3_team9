#pragma once

#include "Runtime/CoreUObject/UPrimitiveComponent.h"
#include "Runtime/Geometry/FTransform.h"
#include "Runtime/Math/FVector2.h"
#include "Runtime/CoreUObject/UTextInstanceComponent.h"
#include "Runtime/Rendering/FRenderer.h"

struct FCamera;
class FGizmo;
class FGrid;
class AActor;

class FRenderView final {
	FRenderer& Renderer;

public:
	FRenderView(FRenderer& Renderer);
	FRenderer& GetRenderer() { return Renderer; }
	const FRenderer& GetRenderer() const { return Renderer; }
	FRenderView(const FRenderView&) = delete;
	FRenderView& operator=(const FRenderView&) = delete;

	void Render(const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, UPrimitiveComponent* Rendered, FSceneView& sceneView, bool bHighlighted = false);
	void RenderGizmo(const FTransform& Transform, const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, const FGizmo& Gizmo);
	void RenderGrid(const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, FGrid& Grid);
	void RenderLine(const FVector& Start, const FVector& End, const FVector4& Color);
	void RenderBoxCenterExtent(const FVector& Center, const FVector& Extent, const FVector4& Color);
	void RenderBoxMinMax(const FVector& Min, const FVector& Max, const FVector4& Color);
	void RenderQuad(const FVector& A, const FVector& B, const FVector& C, const FVector& D, const FVector4& Color);
	void RenderSphere(const FVector& Center, float Radius, const FVector4& Color, uint32 Segments = 16);
	void RenderUUIDText(const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, UTextInstanceComponent* textcomp, const FSceneView& SceneView);
	
	void RenderOutline(const FCamera& Camera, const AActor* SelectedActor);
	void DrawStencilMask(const FCamera& Camera, const AActor* SelectedActor);
	void RenderPostProcess(const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, AActor* SelectedActor);
	void RenderVerticetoline();

	void SetViewportUV(FVector2 TopLeftUV, FVector2 LengthUV);
	void SetRenderMode(EViewModeIndex InMode);
	void UpdateLightConstants(FLightConstants& Constants, const EViewModeIndex InMode);
	void DrawInstances(const FCamera& Camera);
	void ClearTextInstances();
	void FlushLineBatch(const FMatrix& ViewProjection);
};
