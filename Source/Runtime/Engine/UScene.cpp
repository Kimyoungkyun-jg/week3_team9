#include "UScene.h"

#include "Runtime/CoreUObject/USceneComponent.h"
#include "Runtime/CoreUObject/UPrimitiveComponent.h"
#include "Runtime/Core/TArray.h"
#include <string>
#include "Runtime/CoreUObject/UClass.h"
#include "Runtime/CoreUObject/FReferenceCollector.h"

void UScene::RegisterComponent(USceneComponent& Component)
{
	Component.OnRegister(*this);
	Components.push_back(&Component);
}

void UScene::UnregisterComponent(USceneComponent& Component)
{
	Component.OnUnregister(*this);
	// TODO: std 말고 TArray erase 사용
	std::erase(Components, &Component);
}

TArray<UPrimitiveComponent*> UScene::GetPrimitiveComponents() const
{
	// TODO: 매우 비효율적인 방식...
	TArray<UPrimitiveComponent*> PrimitiveComponents{};
	for (const auto* Component : Components)
	{
		if (const auto* PrimitiveComponent = Component->Cast<UPrimitiveComponent>())
		{
			PrimitiveComponents.push_back(const_cast<UPrimitiveComponent*>(PrimitiveComponent));
		}
	}
	return PrimitiveComponents;
}

json::JSON UScene::Serialize() const
{
	json::JSON result;
	result["Version"] = 1;
	result["NextUUID"] = NextUUID;

	for(UObject* object : Components)
	{
		if (object == nullptr)
			continue;

		json::JSON objectJson = object->Serialize();
		if(!objectJson.IsNull())
		{
			result["Primitives"][std::to_string(object->GetUUID())] = objectJson;
		}
	}


	return result;
}

void UScene::CreateFromJson(json::JSON data)
{
	Deserialize(data);
	


}

void UScene::AddReferencedObjects(FReferenceCollector& Collector)
{
	UObject::AddReferencedObjects(Collector);

	for (USceneComponent* Component : Components)
		Collector.AddReferencedObject(Component);
}

bool UScene::Deserialize(const json::JSON& data)
{
	if (data.hasKey("NextUUID"))
		NextUUID = data.at("NextUUID").ToInt();

	Components.clear();

	if (!data.hasKey("Primitives"))   // 빈 씬이면 여기서 정상 종료
		return true;

	json::JSON primitivesJson = data.at("Primitives");
	for (auto& primitiveJson : primitivesJson.ObjectRange())
	{
		uint32 uuid = std::stoi(primitiveJson.first);
		json::JSON usceneComponentData = primitiveJson.second;

		if (!usceneComponentData.hasKey("Type"))
			continue;

		UClass* _class = UClass::FindClassWithDisplayName(usceneComponentData.at("Type").ToString());
		if (_class == nullptr)    
			continue;

		UObject* obj = _class->CreateDefaultObject();
		USceneComponent* component = static_cast<USceneComponent*>(obj);

		component->Deserialize(usceneComponentData);
		component->SetUUID(uuid);
		RegisterComponent(*component);
	}
	return true;
}
