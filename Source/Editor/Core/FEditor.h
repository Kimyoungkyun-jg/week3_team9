#pragma once

#include "Editor/EditorViewport/FEditorViewport.h"
#include "Editor/Gizmo/FGizmo.h"
#include "Editor/Grid/FGrid.h"
#include "Runtime/Core/IntTypes.h"
#include "Runtime/Core/TArray.h"
#include "Runtime/CoreUObject/UObject.h"
#include "Runtime/Engine/USceneManager.h"


enum class EEditorPrimitiveType : uint8 {
  Cube,
  Cylinder,
  Sphere, // TODO: USphereComp / 스피어 메시 미구현 - 현재 스폰 불가
};

class FEditor final {
public:
  FTransform SelectedTransform;
  FVector SelectedEulerDegDisplay;

public:
  void Initialize(USceneManager *SceneManager);

  void Process();

  void NewScene();
  void SaveScene(const FString &Path);
  void LoadScene(const FString &Path);
  bool CheckSceneExists();

  void AddViewport(FEditorViewport Viewport);
  void DeleteViewport(int32 IndexOfViewport);
  FEditorViewport *GetActiveViewport(); // TODO: 임시로 0번 반환

  bool SelectActor(AActor *Actor);
  void UnSelectActor();
  AActor *GetSelectedActor() const { return SelectedActor; }
  [[nodiscard]] bool ActorSelected() const { return SelectedActor != nullptr; }
  [[nodiscard]] bool ObjectSelected() const { return SelectedActor != nullptr; }

  [[nodiscard]] TArray<FEditorViewport> &GetViewports() {
    return EditorViewports;
  }
  [[nodiscard]] UScene *GetCurrentScene() const {
    return SceneManager ? SceneManager->CurrentScene : nullptr;
  }
  UPrimitiveComponent *SpawnPrimitive(EEditorPrimitiveType Type);
  // 피킹 등에서 현재 씬의 렌더링 대상 컴포넌트가 필요할 때 사용
  [[nodiscard]] TArray<UPrimitiveComponent *> GetPrimitiveComponents() const;
  FGizmo &GetGizmo() { return Gizmo; }
  FGrid &GetGrid() { return Grid; }
  FRenderResourceLibrary *GetRendererLibrary();

  void ClearSelectionForGC();

  float GetCameraSensitivity() const { return CameraSensitivity; }
  void SetCameraSensitivity(float Value);

private:
  USceneManager *SceneManager =
      nullptr; // 씬을 다중으로 가질 수 있도록 구조개선 가능-이경우 에디터쪽에
               // 클래스를 추가해 씬과 FEditorViewport들을 연관
  TArray<FEditorViewport> EditorViewports;

  // TODO: 이게 여기에 있으면 안됨... 구조 리팩토링 할 것..
  float CameraSensitivity = 0.5f;

  FGizmo Gizmo;
  FGrid Grid;
  AActor *SelectedActor = nullptr;
};
