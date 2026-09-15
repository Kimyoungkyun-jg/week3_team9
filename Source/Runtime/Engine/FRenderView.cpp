#include "FRenderView.h"

#include "Editor/Gizmo/FGizmo.h"
#include "Editor/Grid/FGrid.h"
#include "Runtime/CoreUObject/UBillBoardComp.h"
#include "Runtime/CoreUObject/UClass.h"
#include "Runtime/Engine/FCamera.h"
#include "Runtime/Math/FVector2.h"
#include "Runtime/Rendering/FRenderer.h"
#include "Runtime/Rendering/ShaderConstants.h"
#include "Runtime/Actors/AActor.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include <fstream>

FRenderView::FRenderView(FRenderer &Renderer) : Renderer(Renderer) {}

void FRenderView::Render(const FCamera &Camera, FVector2 TopLeftUV,
                         FVector2 LengthUV, UPrimitiveComponent *Rendered,
                         bool bHighlighted) {
  if (!Rendered) {
    return;
  }

  Renderer.SetViewportUV(TopLeftUV, LengthUV);
  Rendered->Render(Renderer, Camera, bHighlighted);

  Renderer.DrawInstances(Camera);
  Renderer.ClearTextInstances();
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

void FRenderView::RenderLine(const FVector &Start, const FVector &End,
                             const FVector4 &Color) {
  FLineBatcher &LineBatcher = Renderer.GetLineBatcher();
  LineBatcher.DrawLine(Start, End, Color);
}

void FRenderView::RenderBoxCenterExtent(const FVector &Center,
                                        const FVector &Extent,
                                        const FVector4 &Color) {
  FLineBatcher &LineBatcher = Renderer.GetLineBatcher();
  LineBatcher.DrawBoxCenterExtent(Center, Extent, Color);
}

void FRenderView::RenderBoxMinMax(const FVector &Min, const FVector &Max,
                                  const FVector4 &Color) {
  FLineBatcher &LineBatcher = Renderer.GetLineBatcher();
  LineBatcher.DrawBoxMinMax(Min, Max, Color);
}

void FRenderView::RenderSphere(const FVector &Center, float Radius,
                               const FVector4 &Color, uint32 Segments) {
  FLineBatcher &LineBatcher = Renderer.GetLineBatcher();
  LineBatcher.DrawSphere(Center, Radius, Color, Segments);
}

void FRenderView::RenderUUIDText(const FCamera &Camera, FVector2 TopLeftUV,
                                 FVector2 LengthUV,
                                 UTextInstanceComponent *textcomp) {
  if (!textcomp || !textcomp->GetMesh() || !textcomp->GetMaterial()) {
    return;
  }
  Renderer.SetViewportUV(TopLeftUV, LengthUV);
  Renderer.ClearDepth();
  textcomp->Render(Renderer, Camera, false);
  Renderer.DrawTextInstances(Camera, textcomp->GetMesh()->MeshId,
                             textcomp->GetMaterial()->MaterialId);
  Renderer.ClearTextInstances();
}

void FRenderView::RenderOutline(const FCamera& Camera, const AActor* SelectedActor) {
    DrawStencilMask(Camera, SelectedActor);
    Renderer.RenderOutline();
}

void FRenderView::DrawStencilMask(const FCamera& Camera, const AActor* SelectedActor)
{
    if (!SelectedActor) return;

    // Stencil write는 editor scene의 color target과 DSV가 함께 바인딩된
    // 상태에서만 유효하다. 이전 패스의 OM 상태에 의존하지 않는다.
    Renderer.BindEditorViewportRenderTargets();

    USceneComponent* RootComp = SelectedActor->GetRootComponent();
    if (!RootComp) return;

    UPrimitiveComponent* PrimComp = RootComp->Cast<UPrimitiveComponent>();
    if (!PrimComp || !PrimComp->GetMesh()) return;

    // 메쉬 모델 행렬과 상수 버퍼 준비
    const FMatrix ModelMatrix = PrimComp->GetModelMatrix();
    FObjectConstants Constants{};
    Constants.World = ModelMatrix;
    Constants.MVP = Constants.World * Camera.CreateViewProjectionMatrix();

    // 마스크용 머티리얼로 스텐실 기록
    auto OutlineMaterial = FRenderResourceLibrary::Get().GetMaterial(EMaterialID::Outline);
    if (OutlineMaterial) {
        OutlineMaterial->GetPipeline()->SetStencilRef(1);
        Renderer.Draw(*PrimComp->GetMesh(), *OutlineMaterial, Constants, 0, false);
    }
}

void FRenderView::RenderPostProcess(const FCamera& Camera, FVector2 TopLeftUV, FVector2 LengthUV, AActor* SelectedActor)
{
    // 에디터 뷰포트 설정 후 후처리 수행
    Renderer.SetViewportUV(TopLeftUV, LengthUV);
    RenderOutline(Camera, SelectedActor);
}

void FRenderView::ChangeToBackBuffer()
{
    ID3D11RenderTargetView* BackBufferRTV = Renderer.GetBackBuffer();
    Renderer.GetContext()->OMSetRenderTargets(1, &BackBufferRTV, nullptr);
}
