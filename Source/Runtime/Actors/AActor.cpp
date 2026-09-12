#include "AActor.h"
#include "Runtime/CoreUObject/FReferenceCollector.h"
#include "Runtime/CoreUObject/UClass.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/Engine/UScene.h"
#include "Runtime/Engine/FArchive.h"

IMPLEMENT_UCLASS(AActor, UObject)

void AActor::Initialize(UObject* Context)
{
    Super::Initialize(Context);
    Owner = Context ? Context->Cast<UScene>() : nullptr;
}

void AActor::Release()
{
    if (Owner)
    {
        Owner->RemoveActor(this);
        Owner = nullptr;
    }

    while (!AttachedComp.empty())
    {
        USceneComponent* Component = AttachedComp.back();
        std::erase(AttachedComp, Component);

        if (RootComponent == Component)
        {
            RootComponent = nullptr;
        }

        DestroyObject(Component);
    }

    if (RootComponent)
    {
        USceneComponent* RemainingRoot = RootComponent;
        RootComponent = nullptr;
        DestroyObject(RemainingRoot);
    }

    Super::Release();
}

void AActor::Serialize(FArchive& Archive) const
{
    Super::Serialize(Archive);

    if (RootComponent)
    {
        FArchive RootArchive{};
        RootComponent->Serialize(RootArchive);
        Archive.SetArchive("RootComponent", RootArchive);
    }
    else
    {
        Archive.SetNull("RootComponent");
    }
}

void AActor::Deserialize(const FArchive& Archive)
{
    Super::Deserialize(Archive);

    if (Archive.IsNull("RootComponent"))
    {
        RootComponent = nullptr;
    }
    else
    {
        FArchive RootComponentArchive = Archive.GetArchive("RootComponent");
        UClass* ClassType = UClass::FindByName(RootComponentArchive.GetString("Type"));

        if (ClassType == nullptr) { RootComponent = nullptr; return; }

        CreateRootComponent(ClassType);
        RootComponent->Deserialize(RootComponentArchive);
    }
}

void AActor::CreateRootComponent(UClass* ClassType)
{
    if (RootComponent) { return; }

    UObject* Object = NewObject(ClassType);
    RootComponent = Object->Cast<USceneComponent>();
    RootComponent->Initialize(this);
    AttachedComp.push_back(RootComponent);
}

void AActor::SetRootComponent(USceneComponent *InRootComponent) {
    if (RootComponent == InRootComponent) { return; }

    // 기존 루트 컴포넌트가 있었다면 정리 및 트랜스폼 보관
    if (RootComponent) {
        if (Owner) {
            RootComponent->UnregisterComponentFromScene(*Owner); // 씬 렌더 큐에서 제거
        }
        
        std::erase(AttachedComp, RootComponent); // 액터 참조 목록에서 제거
    }

    // 새 루트 컴포넌트 장착
    RootComponent = InRootComponent;
    if (RootComponent) {
        AttachedComp.push_back(RootComponent);
        InRootComponent->Initialize(this);
        
        // 씬에 이미 스폰된 액터라면 새 루트 컴포넌트도 즉시 씬에 등록
        if (Owner) {
            RootComponent->RegisterComponentWithScene(*Owner);
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
  Addcomp->Initialize(this);

  if (Owner) // 씬이 존재하면 바로 register
  {
    Addcomp->RegisterComponentWithScene(*Owner);
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
