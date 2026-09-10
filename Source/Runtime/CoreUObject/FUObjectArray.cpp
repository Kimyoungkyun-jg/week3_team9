#include "FUObjectArray.h"

#include <cassert>

void FUObjectArray::AddObject(UObject* Object)
{
	Object->InternalIndex = static_cast<uint32>(Objects.size());
	Object->UUID = AcquireUUID();
	Objects.push_back(Object);
}

void FUObjectArray::RemoveObject(UObject* Object)
{
	const uint32 Index = Object->InternalIndex;
	assert(Index < Objects.size() && Objects[Index] == Object);

	UObject* LastObject = Objects.back();

	Objects[Index] = LastObject;
	LastObject->InternalIndex = Index;

	Objects.pop_back();
}

void FUObjectArray::DestroyObject(UObject* Object) {
	if (Object == nullptr) return;

	RemoveObject(Object);
	delete Object; // 오버라이드해서 통계 구현 필요
}