#include "UObject.h"
#include "UClass.h"
#include "UObjectGlobals.h"

IMPLEMENT_ROOT_UCLASS(UObject)

json::JSON UObject::Serialize() const
{
	return json::JSON();
}

bool UObject::Deserialize(const json::JSON& data)
{
	return false;
}

void UObject::AddReferencedObjects(FReferenceCollector& Collector)
{
}

void* UObject::operator new(std::size_t Size)
{
	void* Memory = ::operator new(Size);
	TotalAllocationBytes += Size;
	++TotalAllocationCount;

	return Memory;
}

void UObject::operator delete(void* Memory, std::size_t Size) noexcept
{
	if (Memory == nullptr) return;

	TotalAllocationBytes -= Size;
	--TotalAllocationCount;

	::operator delete(Memory);
}

void* UObject::operator new(std::size_t Size, std::align_val_t Alignment)
{
	void* Memory = ::operator new(Size, Alignment);

	TotalAllocationBytes += Size;
	++TotalAllocationCount;

	return Memory;
}

void UObject::operator delete(void* Memory, std::size_t Size, std::align_val_t Alignment) noexcept
{
	if (Memory == nullptr) return;

	TotalAllocationBytes -= Size;
	--TotalAllocationCount;

	::operator delete(Memory, Alignment);
}
