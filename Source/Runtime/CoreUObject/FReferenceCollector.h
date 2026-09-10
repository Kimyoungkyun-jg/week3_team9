#pragma once

#include "Runtime/Core/TArray.h"
#include <unordered_set>

class UObject;

class FReferenceCollector final {
public:
	void AddReferencedObject(UObject* Object);
	void ProcessReferences();

	[[nodiscard]]
	bool bIsReferenced(const UObject* Object) const;

private:
	std::unordered_set<const UObject*> ReferencedObjects;
	TArray<UObject*> PendingObjects;
};