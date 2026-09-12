#include "AActor.h"
#include "Runtime/CoreUObject/FReferenceCollector.h"
#include "Runtime/CoreUObject/UClass.h"
#include "Runtime/CoreUObject/UObjectGlobals.h"
#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/Engine/UScene.h"
#include "Runtime/Engine/FArchive.h"

IMPLEMENT_UCLASS(AActor, UObject)

void AActor::Initialize()
{
    Super::Initialize();
    Owner = nullptr;
    bHasBegunPlay = false;
}

void AActor::Release()
{
    UScene* RegisteredScene = Owner;
    if (bHasBegunPlay) { EndPlay(); }
    if (Owner) { Unregister(); }
    if (RegisteredScene) { RegisteredScene->RemoveActor(this); }

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
    if (!RootComponent)
    {
        DestroyObject(Object);
        return;
    }

    RootComponent->ActorOwner = this;
    RootComponent->SetupAttachment(nullptr);
    RootComponent->Initialize();
    AttachedComp.push_back(RootComponent);

    if (Owner) { RootComponent->Register(*Owner); }
    if (bHasBegunPlay) { RootComponent->BeginPlay(); }
}

void AActor::SetRootComponent(USceneComponent *InRootComponent) {
    if (RootComponent == InRootComponent) { return; }

    // 기존 루트 컴포넌트가 있었다면 정리 및 트랜스폼 보관
    if (RootComponent) {
        if (RootComponent->HasBegunPlay()) { RootComponent->EndPlay(); }
        if (RootComponent->IsRegistered()) { RootComponent->Unregister(); }
        
        std::erase(AttachedComp, RootComponent); // 액터 참조 목록에서 제거
    }

    // 새 루트 컴포넌트 장착
    RootComponent = InRootComponent;
    if (RootComponent) {
        RootComponent->ActorOwner = this;
        RootComponent->SetupAttachment(nullptr);
        AttachedComp.push_back(RootComponent);
        InRootComponent->Initialize();
        
        if (Owner) { RootComponent->Register(*Owner); }
        if (bHasBegunPlay) { RootComponent->BeginPlay(); }
    }
}

void AActor::AddComponent(USceneComponent *Addcomp) {
  if (Addcomp == nullptr) {
    return;
  }

  if (RootComponent == nullptr) {
    RootComponent = Addcomp;
    Addcomp->SetupAttachment(nullptr);
  }
  else if (Addcomp->GetSceneOwner() == nullptr) {
    Addcomp->SetupAttachment(RootComponent);
  }

  Addcomp->ActorOwner = this;
  AttachedComp.push_back(Addcomp);
  Addcomp->Initialize();

  if (Owner) { Addcomp->Register(*Owner); }
  if (bHasBegunPlay) { Addcomp->BeginPlay(); }
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

void AActor::Register(UScene& Scene) {
  if (Owner == &Scene) { return; }
  if (Owner) { Unregister(); }

  Owner = &Scene;
  for (USceneComponent* Component : AttachedComp) {
    if (Component) { Component->Register(Scene); }
  }
}

void AActor::BeginPlay() {
  if (!Owner || bHasBegunPlay) { return; }

  bHasBegunPlay = true;
  for (USceneComponent* Component : AttachedComp) {
    if (Component) { Component->BeginPlay(); }
  }
}

void AActor::Update(float DeltaTime) {
  if (!bHasBegunPlay) { return; }

  for (USceneComponent *Component : AttachedComp) {
    if (Component) {
      Component->Update(DeltaTime);
    }
  }
}

void AActor::SetColor(const FVector& InColor) {
  if (auto* PrimComp = RootComponent ? RootComponent->Cast<UPrimitiveComponent>() : nullptr) {
    PrimComp->SetColor(InColor);
  }
}

FVector AActor::GetColor() const {
  if (auto* PrimComp = RootComponent ? RootComponent->Cast<UPrimitiveComponent>() : nullptr) {
    return PrimComp->GetColor();
  }
  return FVector{1.0f, 1.0f, 1.0f};
}

void AActor::EndPlay() {
  if (!bHasBegunPlay) { return; }

  for (auto It = AttachedComp.rbegin(); It != AttachedComp.rend(); ++It) {
    if (*It) { (*It)->EndPlay(); }
  }
  bHasBegunPlay = false;
}

void AActor::Unregister() {
  if (bHasBegunPlay) { EndPlay(); }
  if (!Owner) { return; }

  for (auto It = AttachedComp.rbegin(); It != AttachedComp.rend(); ++It) {
    if (*It) { (*It)->Unregister(); }
  }
  Owner = nullptr;
}

void AActor::Destroy() {
  if (Owner) {
    Owner->DestroyActor(this);
    return;
  }
  DestroyObject(this);
}
