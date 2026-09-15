#include "FEditorApplication.h"

#include "Runtime/CoreUObject/FGarbageCollector.h"
#include "Runtime/CoreUObject/FReferenceCollector.h"
#include "Runtime/CoreUObject/UAnimatedBillboardComp.h"
#include "Runtime/CoreUObject/UBillBoardComp.h"
#include "Runtime/CoreUObject/UCubeComp.h"
#include "Runtime/CoreUObject/UCylinderComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/CoreUObject/UPrimitiveComponent.h"
#include "Runtime/CoreUObject/USpotLightComponent.h"
#include "Runtime/CoreUObject/UTextComponent.h"
#include "Runtime/Engine/FRayCastingManager.h"
#include "Runtime/Geometry/FAxisAlignedBoundingBox.h"
#include "Runtime/Math/FMatrix.h"
#include "Runtime/Rendering/FMesh.h"
#include <Windows.h>


#include "Runtime/Engine/FSceneView.h"

#include "Runtime/Actors/AActor.h"
#include "Runtime/Actors/AInstancingActor.h"
#include "Runtime/Actors/TestTextActor.h"
#include "Runtime/CoreUObject/UPlaneComp.h"
#include "Runtime/CoreUObject/USphereComp.h"
#include "Runtime/CoreUObject/UTextComponent.h"

#include "Editor/Visualizer/IVisualizer.h"


void FEditorApplication::Initialize_ImguiWin32DX11(
    HWND &Window, ID3D11Device *Device, ID3D11DeviceContext *Context) {
  ImguiManager.Initialize_ImplWin32DX11(Window, Device, Context);
}

void FEditorApplication::Initialize_Runtime(USceneManager *SceneManager,
                                            FRenderView *RenderView) {
  this->RenderView = RenderView;
  this->SceneManager = SceneManager;
  this->CurrentScene = SceneManager->CurrentScene;

  Editor.Initialize(SceneManager);

  Editor.AddViewport(FEditorViewport{});

  Editor.LoadState();


  // Editor.LoadScene("");
}

void FEditorApplication::Shutdown() {
  Editor.Shutdown();
}

/// <summary>
/// return value: if scene is pre-existing, returns true
/// if scene was not existing, returns false
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
bool FEditorApplication::CheckSceneExistsAndInitializeIfNotExists(
    const FString &path) {
  if (Editor.CheckSceneExists())
    return true;
  else {
    if (path == "")
      Editor.NewScene();
    else
      Editor.LoadScene(path);
    return false;
  }
}

void FEditorApplication::Update(float DeltaTime) {
  BeginFrame();
  Tick(DeltaTime);
}

void FEditorApplication::BeginFrame() { ImguiManager.NewFrame(); }

void FEditorApplication::Tick(float DeltaTime) {
  ToolBar.Process(Editor, ConsoleWindow, ControlPanelWindow, PropertyWindow);
  EditorViewportWindow.Process(Editor, DeltaTime);
  WorldOutliner.Process(Editor);
  ControlPanelWindow.Process(Editor);
  PropertyWindow.Process(Editor);
  ConsoleWindow.Process(Editor);
  ContentsDrawer.Process(Editor);
  Editor.Process();
}

void FEditorApplication::Render() {
  const TArray<FEditorViewport> &EditorViewports = Editor.GetViewports();

  for (auto &EditorViewport : EditorViewports)
  {
      //VP행렬을 매번 계산하는걸 방지하기 위해 FSceneView 사용
      FSceneView sceneview
      {
          EditorViewport.ViewportCamera,
          EditorViewport.ViewportCamera.CreateViewProjectionMatrix()
      };

    if (RenderView) 
    {
      RenderView->SetRenderMode(EditorViewport.ViewMode);
      RenderView->UpdateLightConstants(Editor.GlobalLight, EditorViewport.ViewMode); // globallgiht udpate
    }

    RenderView->RenderGrid(EditorViewport.ViewportCamera,
                           EditorViewport.TopLeftUV, EditorViewport.LengthUV,
                           Editor.GetGrid()); // 그리드 그리기

    RenderView->SetViewportUV(EditorViewport.TopLeftUV, EditorViewport.LengthUV);

    for (auto& PrimitiveComponent : SceneManager->CurrentScene->GetRenderComponents())
    {
        if (!PrimitiveComponent) continue;

        if (!EditorViewport.HasShowFlag(PrimitiveComponent->GetShowFlag()))
        {
            continue;
        }


        bool bSelected = true;

        if (!PrimitiveComponent) { bSelected = false; }
        else if (!PrimitiveComponent->GetActorOwner()) { bSelected = false; }
        else if (PrimitiveComponent->GetActorOwner() != Editor.GetSelectedActor()) { bSelected = false; }


        RenderView->Render
        (
            EditorViewport.ViewportCamera,
            EditorViewport.TopLeftUV,
            EditorViewport.LengthUV,
            PrimitiveComponent,
            sceneview,
            bSelected
        );
    }


    
    RenderView->DrawInstances(sceneview.Camera);
    RenderView->ClearTextInstances();


    if (Editor.ObjectSelected())
    {
        // 선택된 물체에 대해서 Visualizer 수행
        USceneComponent* RootComp = Editor.GetSelectedActor()->GetRootComponent();
        UPrimitiveComponent* PrimComp = RootComp->Cast<UPrimitiveComponent>();

        if (PrimComp && RenderView)
        {
            UClass* ClassType = PrimComp->GetClass();
            IVisualizer* Visualizer = VisualizerRegistry.FindVisualizer(ClassType);
            Visualizer->Draw(*PrimComp, *RenderView, EditorViewport.ViewportCamera);
        }
    }


    RenderView->FlushLineBatch(sceneview.ViewProj); // line batch 일괄 flush


    if (Editor.ObjectSelected())
    {
      // 기즈모 그리기
      RenderView->RenderGizmo(
          Editor.SelectedTransform, EditorViewport.ViewportCamera,
          EditorViewport.TopLeftUV, EditorViewport.LengthUV, Editor.GetGizmo());
    }

    // 선택 객체 하이라이트 렌더
  }
  ImguiManager.RenderUI();
}

void FEditorApplication::OnWindowSize(UINT Width, UINT Height) {
  // 뷰포트 종횡비 갱신
  for (auto &Viewport : Editor.GetViewports()) {
    const FVector2 SizePixels =
        Viewport.LengthUV *
        FVector2{static_cast<float>(Width), static_cast<float>(Height)};

    auto &Camera = Viewport.ViewportCamera;
    Camera.Projection.Aspect = SizePixels.X / SizePixels.Y;
  }
}

void FEditorApplication::CollectGarbage() {
  FGarbageCollector::Get().CollectGarbage();
}
