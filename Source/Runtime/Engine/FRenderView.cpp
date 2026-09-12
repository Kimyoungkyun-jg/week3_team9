#include "FRenderView.h"

#include "Editor/Gizmo/FGizmo.h"
#include "Editor/Grid/FGrid.h"
#include "Runtime/Engine/FCamera.h"
#include "Runtime/Math/FVector2.h"
#include "Runtime/Rendering/FRenderer.h"
#include "Runtime/Rendering/ShaderConstants.h"
#include "Runtime/CoreUObject/UBillBoardComp.h"
#include "Runtime/CoreUObject/UClass.h"
#include <fstream>

FRenderView::FRenderView(FRenderer &Renderer) : Renderer(Renderer) {}

void FRenderView::Render(const FCamera &Camera, FVector2 TopLeftUV, FVector2 LengthUV, UPrimitiveComponent *Rendered, bool bHighlighted) {
  // 렌더 대상과 필수 자원 유효성 검증
  if (!Rendered || !Rendered->GetMesh() || !Rendered->GetMaterial()) {
    return;
  }

  Renderer.SetViewportUV(TopLeftUV, LengthUV);

  const FMatrix VP = Camera.CreateViewProjectionMatrix();

  FObjectConstants Constants;
  Constants.MVP = Rendered->GetRenderMatrix(Camera) * VP;
    if (auto* BBcomp = Rendered->Cast<UBillBoardComp>()) 
    {
        BBcomp->CalculateRotate(Camera, Constants);//빌보드일때 바라보는 계산
        BBcomp->UpdateUVinfo(Constants);
    }

    if (bHighlighted) {
        Constants.ColorOverride = FVector{ 1.0f, 1.0f, 1.0f };
        Constants.ColorOverrideAmount = 0.5f;
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
