#include "FGarbageCollector.h"

#include "FReferenceCollector.h"
#include "FUObjectArray.h"

#include <algorithm>

void FGarbageCollector::AddRoot(UObject* Object)
{
	if (Object == nullptr) return;

	if (std::find(RootObjects.begin(), RootObjects.end(), Object) == RootObjects.end())
		RootObjects.push_back(Object);
}

void FGarbageCollector::RemoveRoot(UObject* Object)
{
	std::erase(RootObjects, Object);
}

void FGarbageCollector::CollectGarbage(const TFunction<void(const FReferenceCollector&)>& BeforeSweep)
{
	FReferenceCollector Collector;

	for (UObject* Root : RootObjects)
		Collector.AddReferencedObject(Root);

	// mark (발견된 인스턴스들이 refernced배열과 pending배열을 계속 추가해나감. 콜렉터의 Process와 오브젝트의 AddReferenced의 대결)
	Collector.ProcessReferences();

	if (BeforeSweep) BeforeSweep(Collector); // 시스템에 감지되지 않는 참조 제거(헤더파일 참조)

	// sweep
	FUObjectArray& ObjectArray = FUObjectArray::Get();

	uint32 Index = 0;

	while (Index < ObjectArray.GetNumObjects()) {
		UObject* Object = ObjectArray.GetObjectByIndex(Index);
		if (Collector.bIsReferenced(Object))
			++Index;
		else
			ObjectArray.DestroyObject(Object);
	}
}
