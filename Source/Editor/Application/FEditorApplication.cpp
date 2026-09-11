#include "FEditorApplication.h"

#include "Runtime/CoreUObject/FGarbageCollector.h"
#include "Runtime/CoreUObject/FReferenceCollector.h"
#include "Runtime/CoreUObject/UCubeComp.h"
#include "Runtime/CoreUObject/UBillBoardComp.h"
#include "Runtime/CoreUObject/UCylinderComp.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/Engine/FRayCastingManager.h"
#include <Windows.h>

#include "Runtime/Actors/AActor.h"
#include "Runtime/CoreUObject/UPlaneComp.h"
#include "Runtime/CoreUObject/USphereComp.h"

void FEditorApplication::Initialize_ImguiWin32DX11(
    HWND &Window, ID3D11Device *Device, ID3D11DeviceContext *Context) {
  ImguiManager.Initialize_ImplWin32DX11(Window, Device, Context);
}

void FEditorApplication::Initialize_Runtime(
    FRenderResourceLibrary *RendererLibrary, USceneManager *SceneManager,
    FRenderView *RenderView) {
  this->RenderView = RenderView;
  this->SceneManager = SceneManager;
  this->curScene = SceneManager->CurrentScene;

  Editor.Initialize(RendererLibrary, SceneManager);

	UCubeComp* CubeComp = NewObject<UCubeComp>();
	FTransform& CubeTransform = CubeComp->GetRelativeTransform();
	CubeTransform.Location = FVector{ 1.0f, 1.0f, 0.25f };
	CubeTransform.Rotation = FQuaternion::FromEulerXYZDeg(FVector{ 0.5f, 0.5f, 0.5f });
	CubeTransform.Scale3D = FVector{ 0.5f, 0.5f, 0.5f };

  AActor *Cube =
      curScene->SpawnActor<AActor>(FVector(1.0f, 1.0f, 0.25f), // Location
                                   FVector(0.5f, 0.5f, 0.5f)   // Scale
      );

  Cube->SetRootComponent(CubeComp);

  UBillBoardComp* BillBoardComp = NewObject<UBillBoardComp>();
  FTransform& BillBoardTransform = BillBoardComp->GetRelativeTransform();
  BillBoardTransform.Location = FVector{ 1.0f, 1.0f, 0.25f };
  BillBoardTransform.Rotation = FQuaternion::FromEulerXYZDeg(FVector{ 0.5f, 0.5f, 0.5f });
  BillBoardTransform.Scale3D = FVector{ 0.5f, 0.5f, 0.5f };

  AActor* BillBoard =
      curScene->SpawnActor<AActor>(FVector(1.0f, 1.0f, 0.25f), // Location
          FVector(0.5f, 0.5f, 0.5f)   // Scale
      );

  BillBoard->SetRootComponent(BillBoardComp);


  Editor.SelectActor(Cube);

  FEditorViewport Viewport;
  Viewport.ViewportCamera.Position = FVector{-3.0f, 3.0f, 2.0f};
  Viewport.ViewportCamera.Pitch = -25.0f;
  Viewport.ViewportCamera.Yaw = -45.0f;
  Viewport.TopLeftUV = {0.0f, 0.0f};
  Viewport.LengthUV = {0.7f, 0.7f};
  Editor.AddViewport(Viewport);
  // Editor.LoadScene("");
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
  ControlPanelWindow.Process(Editor);
  PropertyWindow.Process(Editor);
  ConsoleWindow.Process(Editor);

  Editor.Process();
}

void FEditorApplication::Render() {
  const TArray<FEditorViewport> &EditorViewports = Editor.GetViewports();

  for (auto &EditorViewport : EditorViewports) {
    if (RenderView) {
      RenderView->GetRenderer().SetRenderMode(EditorViewport.ViewMode);
    }

    RenderView->RenderGrid(EditorViewport.ViewportCamera,
                           EditorViewport.TopLeftUV, EditorViewport.LengthUV,
                           Editor.GetGrid()); // 그리드 그리기

    if (EditorViewport.HasShowFlag(EEngineShowFlags::SF_Primitives)) {
      for (auto &PrimitiveComponent :
           SceneManager->CurrentScene->GetRenderComponents()) {
        const bool bSelected =
            (PrimitiveComponent && PrimitiveComponent->GetOwner() &&
             PrimitiveComponent->GetOwner() == Editor.GetSelectedActor());

        RenderView->Render(EditorViewport.ViewportCamera,
                           EditorViewport.TopLeftUV, EditorViewport.LengthUV,
                           PrimitiveComponent, bSelected);
      }
    }

    RenderView->GetRenderer().FlushLineBatch(
        EditorViewport.ViewportCamera.CreateViewProjectionMatrix()
    ); //line batch 일괄 flush

    if (Editor.ObjectSelected()) // 기즈모 그리기
    {
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
  FGarbageCollector::Get().CollectGarbage(
      [this](const FReferenceCollector &Collector) {
        AActor *SelectedActor = Editor.GetSelectedActor();

        if (SelectedActor != nullptr && !Collector.bIsReferenced(SelectedActor))
          Editor.ClearSelectionForGC();
      });
}
