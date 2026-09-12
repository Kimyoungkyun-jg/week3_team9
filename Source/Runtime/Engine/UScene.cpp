#include "UScene.h"

#include "Runtime/Core/TArray.h"
#include "Runtime/CoreUObject/FReferenceCollector.h"
#include "Runtime/CoreUObject/UClass.h"
#include "Runtime/CoreUObject/UPrimitiveComponent.h"
#include "Runtime/CoreUObject/USceneComponent.h"
#include <string>

TArray<UPrimitiveComponent *> UScene::GetRenderComponents() const {
  return RenderComponents;
}

json::JSON UScene::Serialize() const {
  json::JSON result;
  result["Version"] = 1;
  result["NextUUID"] = NextUUID;

  for (UObject *object : RenderComponents) {
    if (object == nullptr)
      continue;

    json::JSON objectJson = object->Serialize();
    if (!objectJson.IsNull()) {
      result["Primitives"][std::to_string(object->GetUUID())] = objectJson;
    }
  }

  return result;
}

void UScene::CreateFromJson(json::JSON data) { Deserialize(data); }

void UScene::AddReferencedObjects(FReferenceCollector &Collector) {
  UObject::AddReferencedObjects(Collector);

  // 액터 참조 수집
  for (AActor *Actor : Actors)
    Collector.AddReferencedObject(Actor);

  for (USceneComponent *Component : RenderComponents)
    Collector.AddReferencedObject(Component);
}

void UScene::AddRenderComponent(UPrimitiveComponent *prim) {
  RenderComponents.push_back(prim);
}

void UScene::RemoveRenderComponent(UPrimitiveComponent* prim)
{
	std::erase(RenderComponents, prim);
}

void UScene::DestroyActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	Actor->UnregisterAllComponents(*this);
	std::erase(Actors, Actor);
}

bool UScene::Deserialize(const json::JSON &data) {
  if (data.hasKey("NextUUID"))
    NextUUID = data.at("NextUUID").ToInt();

  RenderComponents.clear();

  if (!data.hasKey("Primitives")) // 빈 씬이면 여기서 정상 종료
    return true;

  json::JSON primitivesJson = data.at("Primitives");
  for (auto &primitiveJson : primitivesJson.ObjectRange()) {
    uint32 uuid = std::stoi(primitiveJson.first);
    json::JSON usceneComponentData = primitiveJson.second;

    if (!usceneComponentData.hasKey("Type"))
      continue;

    UClass *_class = UClass::FindClassWithDisplayName(
        usceneComponentData.at("Type").ToString());
    if (_class == nullptr)
      continue;

    UObject *obj = _class->CreateDefaultObject();
    USceneComponent *component = static_cast<USceneComponent *>(obj);

    component->Deserialize(usceneComponentData);
    component->SetUUID(uuid);
    component->RegisterComponentWithScene(*this);
  }
  return true;
}
