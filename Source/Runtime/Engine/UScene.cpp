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

    // 로드된 컴포넌트도 소유 액터를 가져야 한다.
    // 피킹이 액터 단위로 선택하므로(Owner가 없으면 클릭해도 선택되지 않음),
    // 스폰 경로와 동일하게 액터를 만들어 루트로 붙인다.
    // SetRootComponent가 SetOwner와 씬 등록을 모두 처리하므로
    // RegisterComponentWithScene을 따로 부르면 중복 등록된다.
    AActor *Actor = SpawnActor<AActor>();
    Actor->SetRootComponent(component);

    // SetRootComponent가 이전 루트의 트랜스폼을 새 루트로 옮기므로,
    // 역직렬화는 반드시 그 뒤에 해야 저장된 위치가 유지된다.
    component->Deserialize(usceneComponentData);
    component->SetUUID(uuid);
  }
  return true;
}
