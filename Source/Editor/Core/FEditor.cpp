#include "FEditor.h"
#include "Runtime/Actors/AActor.h"
#include "Runtime/Actors/ACubeActor.h"
#include "Runtime/Actors/ASphereActor.h"
#include "Runtime/Actors/ACylinderActor.h"
#include "Runtime/Actors/ABillboardActor.h"
#include "Runtime/Actors/ASpotlightActor.h"
#include "Runtime/CoreUObject/UCubeComp.h"
#include "Runtime/CoreUObject/UCylinderComp.h"
#include "Runtime/CoreUObject/UObject.h"
#include "Runtime/CoreUObject/USphereComp.h"
#include "Runtime/Engine/FTimeManager.h"
#include "Runtime/Input/FInputManager.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include <numbers>


void FEditor::Initialize(USceneManager *SceneManager) {
  State.ReadFromFile();
  Gizmo.Initialize();
  Grid.Initialize();
  this->SceneManager = SceneManager;
}

void FEditor::Shutdown() {
  SaveState();
  State.FlushToFile();
}

FRenderResourceLibrary *FEditor::GetRendererLibrary() {
  return &FRenderResourceLibrary::Get();
}

void FEditor::Process() {
  // 씬의 액터 업데이트
  
    if (FInputManager::Get().IsKeyDown(VK_DELETE) && SelectedActor)
    {
        AActor* Target = SelectedActor;
        UnSelectActor();
        Target->Destroy();
    }
    
  if (SceneManager && SceneManager->CurrentScene) {
    SceneManager->CurrentScene->Update(FTimeManager::Get().GetDeltaTime());
  }

  if (SelectedActor) {
    SelectedActor->SetTransform(SelectedTransform);
  }

  SaveState();
  State.Tick(FTimeManager::Get().GetDeltaTime());
}

void FEditor::SaveState() {
  const FEditorViewport* Viewport = GetActiveViewport();
  if (!Viewport) { return; }

  const FCamera& Camera = Viewport->ViewportCamera;
  State.SetCameraLocation(Camera.Position);
  State.SetCameraPitch(Camera.Pitch);
  State.SetCameraYaw(Camera.Yaw);
  State.SetCameraFOV(Camera.Projection.FOV);
  State.SetGridCellSize(Grid.GetCellSize());
  State.SetGizmoMode(static_cast<uint8>(Gizmo.Mode));
  State.SetGizmoSpace(static_cast<uint8>(Gizmo.GetSpace()));
  State.SetSelectedActor(SelectedActor ? SelectedActor->GetUUID() : static_cast<uint32>(-1));
}

void FEditor::LoadState()
{
    FEditorViewport* Viewport = GetActiveViewport();
    if (!Viewport) { return; }

    FCamera& Camera = Viewport->ViewportCamera;

    Camera.Position = State.GetCameraLocation();
    Camera.Pitch = State.GetCameraPitch();
    Camera.Yaw = State.GetCameraYaw();
    Camera.Projection.FOV = State.GetCameraFOV();
    Grid.SetCellSize(State.GetGridCellSize());
    Gizmo.Mode = static_cast<EGizmoMode>(State.GetGizmoMode());
    Gizmo.SetGizmoSpace(static_cast<EGizmoSpace>(State.GetGizmoSpace()));
}

void FEditor::NewScene() {
  UnSelectActor();
  SceneManager->SetScene(NewObject<UScene>());
  State.ResetToDefaults();
  LoadState();
}

void FEditor::SaveScene(const FString &Path) { SceneManager->SaveScene(Path); }

void FEditor::LoadScene(const FString &Path) 
{

  // 씬 로드
  SceneManager->LoadScene(Path);
  SelectedActor = nullptr;
}

bool FEditor::CheckSceneExists() {
  if (SceneManager->CurrentScene == nullptr)
    return false;
  return true;
}

void FEditor::AddViewport(FEditorViewport Viewport) {
  EditorViewports.push_back(Viewport);
}

void FEditor::DeleteViewport(int32 IndexOfViewport) {
  EditorViewports.erase(EditorViewports.begin() + IndexOfViewport);
}

FEditorViewport *FEditor::GetActiveViewport() {
  if (EditorViewports.empty()) {
    return nullptr;
  }
  return &EditorViewports[0];
}

bool FEditor::SelectActor(AActor *Actor) {
  if (SelectedActor) {
    UnSelectActor();
  }

  SelectedActor = Actor;
  if (SelectedActor) {
    SelectedTransform = SelectedActor->GetTransform();
    SelectedEulerDegDisplay = SelectedTransform.Rotation.GetEulerXYZ();
  }

  return true;
}

void FEditor::UnSelectActor() {
  if (SelectedActor) {
    SelectedActor->SetTransform(SelectedTransform);
  }
  SelectedActor = nullptr;
}

TArray<UPrimitiveComponent *> FEditor::GetPrimitiveComponents() const {
  if (!SceneManager || !SceneManager->CurrentScene) {
    return {};
  }
  return SceneManager->CurrentScene->GetRenderComponents();
}

void FEditor::ClearSelectionForGC() {
  SelectedActor = nullptr;
  Gizmo.EndInteraction();
  Gizmo.HoveredHandle = EGizmoHandle::None;
}

void FEditor::SpawnActorToCurrentScene(UClass* Type, int Size) {
    if (!SceneManager || !SceneManager->CurrentScene) {
        return;
    }

    if (Size <= 0) { return; }

    for (int i = 0; i < Size; ++i)
    {
        // 오프셋 적용
        static int SpawnSerial = 0;
        const float Offset = 0.25f * static_cast<float>(SpawnSerial++);

        FTransform Transform;
        Transform.Location = FVector{ Offset, 0.0f, 0.0f };
        Transform.Scale3D = FVector{ 0.5f, 0.5f, 0.5f };

        AActor* NewActor = SceneManager->CurrentScene->SpawnActor(Type);
        if (!NewActor) { return; }

        USceneComponent* RootComponent = NewActor->GetRootComponent();
        RootComponent->SetRelativeTransform(Transform);
        NewActor->BeginPlay();
        SelectActor(NewActor);
    }
}
