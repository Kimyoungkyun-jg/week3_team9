#include "FRenderView.h"

#include "Editor/Gizmo/FGizmo.h"
#include "Editor/Grid/FGrid.h"
#include "Runtime/Actors/AActor.h"
#include "Runtime/CoreUObject/UBillBoardComp.h"
#include "Runtime/CoreUObject/UClass.h"
#include "Runtime/Engine/FCamera.h"
#include "Runtime/Engine/FSceneView.h"
#include "Runtime/Math/FVector2.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "Runtime/Rendering/FRenderer.h"
#include "Runtime/Rendering/ShaderConstants.h"
#include <fstream>

FRenderView::FRenderView(FRenderer &Renderer) : Renderer(Renderer) {}

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

void FRenderView::RenderQuad(
    const FVector& A,
    const FVector& B,
    const FVector& C,
    const FVector& D,
    const FVector4& Color
)
{
    FLineBatcher& LineBatcher = Renderer.GetLineBatcher();
    LineBatcher.DrawQuad(A, B, C, D, Color);
}

void FRenderView::RenderSphere(const FVector &Center, float Radius,
                               const FVector4 &Color, uint32 Segments) {
  FLineBatcher &LineBatcher = Renderer.GetLineBatcher();
  LineBatcher.DrawSphere(Center, Radius, Color, Segments);
}

void FRenderView::RenderUUIDText(const FCamera& Camera, FVector2 TopLeftUV,
                                 FVector2 LengthUV, UTextInstanceComponent* textcomp, const FSceneView& SceneView)
{
    if (!textcomp) return;

    Renderer.SetViewportUV(TopLeftUV, LengthUV);
    Renderer.ClearDepth();

    // BuildRenderData()로 Font 기반 인스턴스 데이터 획득 후 드로우
    FRenderData Data = textcomp->BuildRenderData(Camera);
    if (!Data.Instances.empty())
    {
        Renderer.AddTextInstanceArray(Data.Instances, Data.MeshId, Data.MaterialId);
        Renderer.DrawTextInstances(Camera, Data.MeshId, Data.MaterialId);
        Renderer.ClearTextInstances();
    }
}

void FRenderView::RenderOutline(const FCamera &Camera,
                                const AActor *SelectedActor) {
  DrawStencilMask(Camera, SelectedActor);
  Renderer.RenderOutline();
}

void FRenderView::DrawStencilMask(const FCamera& Camera,
                                  const AActor* SelectedActor) {
    if (!SelectedActor) return;

    USceneComponent* RootComp = SelectedActor->GetRootComponent();
    if (!RootComp) return;

    UPrimitiveComponent* PrimComp = RootComp->Cast<UPrimitiveComponent>();
    if (!PrimComp) return;

    // FRenderData에서 MeshId 읽어 ResLib로 실제 Mesh 획득
    const FRenderData& RD = PrimComp->GetRenderData();
    auto Mesh = FRenderResourceLibrary::Get().GetMesh(RD.MeshId);
    if (!Mesh) return;

    const FMatrix ModelMatrix = PrimComp->GetModelMatrix();
    FObjectConstants Constants{};
    Constants.World = ModelMatrix;
    Constants.MVP   = Constants.World * Camera.CreateViewProjectionMatrix();

    auto OutlineMaterial = FRenderResourceLibrary::Get().GetMaterial(EMaterialID::Outline);
    if (OutlineMaterial) {
        OutlineMaterial->GetPipeline()->SetStencilRef(1);
        Renderer.Draw(*Mesh, *OutlineMaterial, Constants, 0, false);
    }
}

void FRenderView::RenderPostProcess(const FCamera &Camera, FVector2 TopLeftUV,
                                    FVector2 LengthUV, AActor *SelectedActor) {
  // 에디터 뷰포트 설정 후 후처리 수행
  Renderer.SetViewportUV(TopLeftUV, LengthUV);
  RenderOutline(Camera, SelectedActor);
}
void FRenderView::SetViewportUV(FVector2 TopLeftUV, FVector2 LengthUV)
{
    Renderer.SetViewportUV(TopLeftUV, LengthUV);
}

void FRenderView::SetRenderMode(EViewModeIndex InMode)
{
    Renderer.SetRenderMode(InMode);
}

void FRenderView::UpdateLightConstants(FLightConstants& Constants, const EViewModeIndex InMode)
{
    Renderer.UpdateLightConstants(Constants, InMode);
}

void FRenderView::DrawInstances(const FCamera& Camera)
{
    Renderer.DrawInstances(Camera);
}

void FRenderView::ClearTextInstances()
{
    Renderer.ClearTextInstances();
}

void FRenderView::FlushLineBatch(const FMatrix& ViewProjection)
{
    Renderer.FlushLineBatch(ViewProjection);
}

void FRenderView::FlushQueue(const FCamera& Camera)
{
    auto& ResLib = FRenderResourceLibrary::Get();

    // Primitive 큐 처리
    for (const FRenderData& Data : RenderQueue.GetPrimRenderQ())
    {
        auto Mesh     = ResLib.GetMesh(Data.MeshId);
        auto Material = ResLib.GetMaterial(Data.MaterialId);
        if (!Mesh || !Material) continue;
        Renderer.Draw(*Mesh, *Material, Data.Constants);
    }

    // Instancing 큐
    if (!RenderQueue.IsInstancingRQEmpty())
    {
        for (const FRenderData& Data : RenderQueue.GetInstancingRenderQ())
        {
            Renderer.AddTextInstanceArray(Data.Instances, Data.MeshId, Data.MaterialId);
        }
        Renderer.DrawInstances(Camera);
        Renderer.ClearTextInstances();
    }

    // Texture 큐: Primitive와 동일하지만 TextureId로 머티리얼 텍스처 교체 후 드로우
    for (const FRenderData& Data : RenderQueue.GetTextureRenderQ())
    {
        auto Mesh     = ResLib.GetMesh(Data.MeshId);
        auto Material = ResLib.GetMaterial(Data.MaterialId);
        if (!Mesh || !Material) continue;

        if (!Data.TextureId.empty())
        {
            auto Tex = ResLib.GetTexture(Data.TextureId);
            if (Tex)
            {
                // 원본 머티리얼을 건드리지 않도록 인스턴스 복사
                auto MatInst = TSharedPtr<FMaterial>(new FMaterial(*Material));
                MatInst->SetTexture(Tex);
                Renderer.Draw(*Mesh, *MatInst, Data.Constants);
                continue;
            }
        }
        Renderer.Draw(*Mesh, *Material, Data.Constants);
    }

    // Text 큐: BuildRenderData()에서 이미 계산된 Instances 배열 사용
    if (!RenderQueue.IsTextRQEmpty())
    {
        const FRenderData& First = RenderQueue.GetTextRenderQ()[0];
        EMeshID     TextMeshId     = First.MeshId;
        EMaterialID TextMaterialId = First.MaterialId;

        for (const FRenderData& Data : RenderQueue.GetTextRenderQ())
        {
            // Font에서 미리 계산된 글자별 쿼드 데이터를 그대로 넘김
            Renderer.AddTextInstanceArray(Data.Instances, Data.MeshId, Data.MaterialId);
        }
        Renderer.DrawTextInstances(Camera, TextMeshId, TextMaterialId);
        Renderer.ClearTextInstances();
    }

    RenderQueue.Clear();
}

