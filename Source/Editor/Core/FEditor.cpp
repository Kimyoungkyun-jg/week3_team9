#include "FEditor.h"
#include "Runtime/Actors/AActor.h"
#include "Runtime/Actors/ACubeActor.h"
#include "Runtime/Actors/ASphereActor.h"
#include "Runtime/Actors/ACylinderActor.h"
#include "Runtime/Actors/ABillboardActor.h"
#include "Runtime/Actors/ASpotlightActor.h"
#include "Runtime/Actors/AInstancingActor.h"
#include "Runtime/CoreUObject/UCubeComp.h"
#include "Runtime/CoreUObject/UCylinderComp.h"
#include "Runtime/CoreUObject/UObject.h"
#include "Runtime/CoreUObject/USphereComp.h"
#include "Runtime/CoreUObject/UInstancePrimitiveComponent.h"
#include "Runtime/Engine/FTimeManager.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include <numbers>
#include <cstdlib>
#include <cmath>


void FEditor::Initialize(USceneManager *SceneManager) {
  State.ReadFromFile();
  Gizmo.Initialize();
  Grid.Initialize();

  

  SelectedActorTextComp = NewObject<UTextInstanceComponent>();
  SelectedActorTextComp->SetInheritRotation(false);
  SelectedActorTextComp->SetMesh(FRenderResourceLibrary::Get().GetMesh(EMeshID::Rect));
  SelectedActorTextComp->SetMaterial(FRenderResourceLibrary::Get().GetMaterial(EMaterialID::SelectedActor_Text));
  SelectedActorTextComp->SetFont();

  this->SceneManager = SceneManager;
}

FRenderResourceLibrary *FEditor::GetRendererLibrary() {
  return &FRenderResourceLibrary::Get();
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
      
    SelectedActorTextComp->SetActorOwner(SelectedActor);
    FTransform RelativeTrans;
    RelativeTrans.Location = FVector{ 0.0f, 0.0f, 1.5f }; 
    SelectedActorTextComp->SetRelativeTransform(RelativeTrans);
    SelectedActorTextComp->SetText(L"UUID : " + std::to_wstring(SelectedActor->GetUUID()));

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

void FEditor::SpawnInstancingToCurrentScene(int Count)
{
    if (!SceneManager || !SceneManager->CurrentScene || Count <= 0) return;

    // 호출마다 색이 달라지도록 HSV 황금비율로 Hue 분산
    static int SpawnBatchIndex = 0;
    const float GoldenRatio = 0.618033988f;
    const float Hue = std::fmod(SpawnBatchIndex * GoldenRatio, 1.0f);
    ++SpawnBatchIndex;

    // HSV → RGB 변환 (S=0.8, V=1.0 고정)
    const float S = 0.8f, V = 1.0f;
    const float H6 = Hue * 6.0f;
    const int   HI = static_cast<int>(H6);
    const float F  = H6 - HI;
    const float P  = V * (1.0f - S);
    const float Q  = V * (1.0f - S * F);
    const float T  = V * (1.0f - S * (1.0f - F));
    FVector4 Color;
    switch (HI % 6)
    {
    case 0: Color = {V, T, P, 1}; break;
    case 1: Color = {Q, V, P, 1}; break;
    case 2: Color = {P, V, T, 1}; break;
    case 3: Color = {P, Q, V, 1}; break;
    case 4: Color = {T, P, V, 1}; break;
    default:Color = {V, P, Q, 1}; break;
    }

    // Actor 1개만 생성
    AActor* NewActor = SceneManager->CurrentScene->SpawnActor(AInstancingActor::StaticClass());
    if (!NewActor) return;

    auto* Comp = NewActor->GetRootComponent()->Cast<UInstancePrimitiveComponent>();
    if (!Comp) return;

    // Count개만큼 랜덤 위치에 같은 색으로 인스턴스 추가
    const float Spread = 20.0f;
    for (int i = 0; i < Count; ++i)
    {
        FVector Pos;
        Pos.X = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * Spread;
        Pos.Y = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * Spread;
        Pos.Z = 0.0f;
        Comp->AddInstance(Pos, Color);
    }

    NewActor->BeginPlay();
    SelectActor(NewActor);
}