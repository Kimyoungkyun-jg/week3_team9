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

    Renderer.SetViewportUV(TopLeftUV, LengthUV);

    const FMatrix VP = Camera.CreateViewProjectionMatrix();
    FTransform& Transform = Rendered->GetRelativeTransform();
	FObjectConstants Constants = { Transform.ToMatrix() * VP};
  
    
    if (bHighlighted) {
        Constants.ColorOverride = FVector{1.0f, 1.0f, 1.0f};
        Constants.ColorOverrideAmount = 0.5f;
    }

    if (auto* BBcomp = Rendered->Cast<UBillBoardComp>()) 
    {
        Constants = BBcomp->CalculateRotate(Camera,0);//빌보드일때 바라보는 계산
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
