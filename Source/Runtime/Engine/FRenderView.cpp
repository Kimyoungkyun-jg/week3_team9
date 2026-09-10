#include "FRenderView.h"

#include "Runtime/Rendering/FRenderer.h"
#include "Runtime/Rendering/ShaderConstants.h"
#include "Runtime/Engine/FCamera.h"
#include "Runtime/Math/FVector2.h"
#include "Editor/Gizmo/FGizmo.h"
#include "Editor/Grid/FGrid.h"

FRenderView::FRenderView(FRenderer& Renderer):
	Renderer(Renderer)
{
}

void FRenderView::Render(const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, UPrimitiveComponent* Rendered, bool bHighlighted)
{
	Renderer.SetViewportUV(TopLeftUV, LengthUV);
	// TODO: 렌더뷰가 렌더러 구현을 알게 해서 여기서 V, P 따로 받고 월드축변환행렬을 곱하거나,
	// 렌더러쪽 UpdateObjectConstants를 Draw함수 안에 숨긴뒤 인수로 M, V, P와 월드축을 받게 하면 렌더뷰도 렌더러 구현 모름
	const FMatrix VP = Camera.CreateViewProjectionMatrix();
	FObjectConstants Constants = { Rendered->RelativeTransform.ToMatrix() *VP };
	if (bHighlighted)
	{
		Constants.ColorOverride = FVector{ 1.0f, 1.0f, 1.0f };
		Constants.ColorOverrideAmount = 0.5f;
	}
	Renderer.Draw(*Rendered->GetMesh(), *Rendered->GetMaterial(), Constants);
}

void FRenderView::RenderGizmo(const FTransform& Transform, const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, const FGizmo& Gizmo)
{
	Renderer.SetViewportUV(TopLeftUV, LengthUV);
	Renderer.ClearDepth();
	Gizmo.Draw(Renderer, Transform, Camera);
}

void FRenderView::RenderGrid(const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, FGrid& Grid)
{
	Renderer.SetViewportUV(TopLeftUV, LengthUV);
	Grid.Draw(Renderer, Camera);

}
