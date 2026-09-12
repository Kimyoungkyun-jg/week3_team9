#include "FEditor.h"
#include "Runtime/Actors/AActor.h"
#include "Runtime/CoreUObject/UCubeComp.h"
#include "Runtime/CoreUObject/UCylinderComp.h"
#include "Runtime/CoreUObject/UObject.h"
#include "Runtime/CoreUObject/USphereComp.h"
#include "Runtime/Engine/FTimeManager.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include <numbers>


void FEditor::Initialize(FRenderResourceLibrary *RendererLibrary,
                         USceneManager *SceneManager) {
  Gizmo.Initialize(*RendererLibrary);
  Grid.Initialize(*RendererLibrary);
  this->RendererLibrary = RendererLibrary;
  this->SceneManager = SceneManager;
}

void FEditor::Process() {
  // 씬의 액터 업데이트
  if (SceneManager && SceneManager->CurrentScene) {
    SceneManager->CurrentScene->Update(FTimeManager::Get().GetDeltaTime());
  }

  if (SelectedActor) {
    SelectedActor->SetTransform(SelectedTransform);
  }
}

void FEditor::NewScene() {
  SelectedActor = nullptr;
  SceneManager->SetScene(NewObject<UScene>());
}

void FEditor::SaveScene(const FString &Path) { SceneManager->SaveScene(Path); }

void FEditor::LoadScene(const FString &Path) {
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

void FEditor::SetCameraSensitivity(float Value)
{
    CameraSensitivity = Value;
}

UPrimitiveComponent* FEditor::SpawnPrimitive(EEditorPrimitiveType Type) {
  if (!SceneManager || !SceneManager->CurrentScene) {
    return nullptr;
  }

  // 액터를 스폰하고 컴포넌트를 루트로 장착
  AActor *NewActor = SceneManager->CurrentScene->SpawnActor<AActor>();

  switch (Type) {
  case EEditorPrimitiveType::Cube:
      NewActor->CreateRootComponent(UCubeComp::StaticClass());
      break;
  case EEditorPrimitiveType::Cylinder:
      NewActor->CreateRootComponent(UCylinderComp::StaticClass());
      break;
  case EEditorPrimitiveType::Sphere:
      NewActor->CreateRootComponent(USphereComp::StaticClass());
      break;
  }

  // 오프셋 적용
  static int SpawnSerial = 0;
  const float Offset = 0.25f * static_cast<float>(SpawnSerial++);
  FTransform Transform
  {
      FVector{ Offset, 0.0f, 0.0f },
      FQuaternion::Identity(),
      FVector{ 0.5f, 0.5f, 0.5f },
  };

  NewActor->GetRootComponent()->SetRelativeTransform(Transform);

  SelectActor(NewActor);
  return NewActor->GetRootComponent()->Cast<UPrimitiveComponent>();
}
