#include "AActor.h"
#include "Runtime/CoreUObject/FReferenceCollector.h"
#include "Runtime/CoreUObject/UClass.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/Engine/UScene.h"

IMPLEMENT_UCLASS(AActor, UObject)

void AActor::SetRootComponent(USceneComponent *InRootComponent) {
  if (RootComponent == InRootComponent)
    return;
  FTransform PrevTransform{};
  bool bHasPrevTransform = false;
  // 기존 루트 컴포넌트가 있었다면 정리 및 트랜스폼 보관
  if (RootComponent) {
    PrevTransform = RootComponent->GetRelativeTransform();
    bHasPrevTransform = true;
    if (OwningScene) {
      RootComponent->UnregisterComponentFromScene(
          *OwningScene); // 씬 렌더 큐에서 제거
    }
    std::erase(AttachedComp, RootComponent); // 액터 참조 목록에서 제거
  }
  // 새 루트 컴포넌트 장착
  RootComponent = InRootComponent;
  if (RootComponent) {
    AttachedComp.push_back(RootComponent);
    InRootComponent->SetOwner(this);
    // 기존 루트가 가지고 있던 액터 트랜스폼을 새 컴포넌트로 이전
    if (bHasPrevTransform) {
      InRootComponent->SetRelativeTransform(PrevTransform);
    }
    // 씬에 이미 스폰된 액터라면 새 루트 컴포넌트도 즉시 씬에 등록
    if (OwningScene) {
      RootComponent->RegisterComponentWithScene(*OwningScene);
    }
  }
}

void AActor::AddComponent(USceneComponent *Addcomp) {
  if (Addcomp == nullptr) {
    return;
  }

  if (RootComponent == nullptr) {
    RootComponent = Addcomp;
  }

  AttachedComp.push_back(Addcomp);
  Addcomp->SetOwner(this);

  if (OwningScene) // 씬이 존재하면 바로 register
  {
    Addcomp->RegisterComponentWithScene(*OwningScene);
  }
}

void AActor::AddReferencedObjects(FReferenceCollector &Collector) {
  UObject::AddReferencedObjects(Collector);

  if (RootComponent) {
    Collector.AddReferencedObject(RootComponent);
  }

  for (USceneComponent *Component : AttachedComp) {
    Collector.AddReferencedObject(Component);
  }
}

void AActor::RegisterAllComponents(UScene &Scene) {
  for (USceneComponent *Comp : AttachedComp) {
    if (Comp) {
      Comp->RegisterComponentWithScene(Scene);
    }
  }
}

void AActor::Update(float DeltaTime) {
  for (USceneComponent *Component : AttachedComp) {
    if (Component) {
      Component->Update(DeltaTime);
    }
  }
}

void AActor::UnregisterAllComponents(UScene &Scene) {
  for (USceneComponent *Comp : AttachedComp) {
    if (Comp) {
      Comp->UnregisterComponentFromScene(Scene);
    }
  }
}

void AActor::UnregisterComponentFromScene(UScene &Scene) {
  UnregisterAllComponents(Scene);
}

void AActor::Destroy() {
  if (OwningScene) {
    // 씬에서 컴포넌트 등록 해제
    UnregisterAllComponents(*OwningScene);
    // 씬의 액터 목록에서 제거
    OwningScene->DestroyActor(this);
    OwningScene = nullptr;
  }
  // 컴포넌트 목록 비우기
  AttachedComp.clear();
  RootComponent = nullptr;
}
