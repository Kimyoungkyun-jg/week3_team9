#include "FRenderView.h"

#include "Editor/Gizmo/FGizmo.h"
#include "Editor/Grid/FGrid.h"
#include "Runtime/Engine/FCamera.h"
#include "Runtime/Math/FVector2.h"
#include "Runtime/Rendering/FRenderer.h"
#include "Runtime/Rendering/ShaderConstants.h"
#include "Runtime/CoreUObject/UBillBoardComp.h"

FRenderView::FRenderView(FRenderer &Renderer) : Renderer(Renderer) {}

void FRenderView::Render(const FCamera &Camera, FVector2 TopLeftUV, FVector2 LengthUV, UPrimitiveComponent *Rendered, bool bHighlighted) {

  // TODO: 렌더뷰가 렌더러 구현을 알게 해서 여기서 V, P 따로 받고
  // 월드축변환행렬을 곱하거나, 렌더러쪽 UpdateObjectConstants를 Draw함수 안에
  // 숨긴뒤 인수로 M, V, P와 월드축을 받게 하면 렌더뷰도 렌더러 구현 모름
  const FMatrix VP = Camera.CreateViewProjectionMatrix();

  FObjectConstants Constants;
    if (auto* BBcomp = Rendered->Cast<UBillBoardComp>()) 
    {
        BBcomp->CalculateRotate(Camera, Constants);//빌보드일때 바라보는 계산
        BBcomp->UpdateUVinfo(Constants);
    }
    
    Renderer.Draw(*Rendered->GetMesh(), *Rendered->GetMaterial(), Constants);
}

void FRenderView::RenderGizmo(const FTransform &Transform,
                              const FCamera &Camera, FVector2 TopLeftUV,
                              FVector2 LengthUV, const FGizmo &Gizmo) {
  Renderer.SetViewportUV(TopLeftUV, LengthUV);
  Renderer.ClearDepth();
  Gizmo.Draw(Renderer, Transform, Camera);
}

void FRenderView::RenderGrid(const FCamera &Camera, FVector2 TopLeftUV,
                             FVector2 LengthUV, FGrid &Grid) {
  Renderer.SetViewportUV(TopLeftUV, LengthUV);
  Grid.DrawLine(Renderer, Camera);
}

void FRenderView::RenderLine(const FVector& Start, const FVector& End, const FVector4& Color)
{
    FLineBatcher& LineBatcher = Renderer.GetLineBatcher();
    LineBatcher.DrawLine(Start, End, Color);
}

void FRenderView::RenderBoxCenterExtent(const FVector& Center, const FVector& Extent, const FVector4& Color)
{
    FLineBatcher& LineBatcher = Renderer.GetLineBatcher();
    LineBatcher.DrawBoxCenterExtent(Center, Extent, Color);
}

void FRenderView::RenderBoxMinMax(const FVector& Min, const FVector& Max, const FVector4& Color)
{
    FLineBatcher& LineBatcher = Renderer.GetLineBatcher();
    LineBatcher.DrawBoxMinMax(Min, Max, Color);
}

void FRenderView::RenderSphere(const FVector& Center, float Radius, const FVector4& Color, uint32 Segments)
{
    FLineBatcher& LineBatcher = Renderer.GetLineBatcher();
    LineBatcher.DrawSphere(Center, Radius, Color, Segments);
}
