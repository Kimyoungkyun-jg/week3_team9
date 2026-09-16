#include "FName.h"
#include "Runtime/Core/FNamePool.h"

FName::FName(const char* CharPtr)
	: FName{ FString{ CharPtr } }
{
}

FName::FName(const FString& Str)
	: Entry{ FNamePool::AddEntry(Str) }
{
}

int32 FName::Compare(const FName& Other) const
{
	if (*this == Other) { return 0; }

	const FString& ThisStr = FNamePool::GetComparisonString(Entry);
	const FString& OtherStr = FNamePool::GetComparisonString(Other.Entry);

	return ThisStr.compare(OtherStr);
}

int32 FName::CompareSensitive(const FName& Other) const
{
	if (
		Entry.DisplayBucketIndex == Other.Entry.DisplayBucketIndex &&
		Entry.DisplayIndex == Other.Entry.DisplayIndex
		)
	{

		return 0;
	}

	const FString& ThisStr = FNamePool::GetDisplayString(Entry);
	const FString& OtherStr = FNamePool::GetDisplayString(Other.Entry);

	return ThisStr.compare(OtherStr);
}

bool FName::operator==(const FName& Other) const
{
	if (
		Entry.ComparisonBucketIndex == Other.Entry.ComparisonBucketIndex &&
		Entry.ComparisonIndex == Other.Entry.ComparisonIndex
		)
	{
		
		return true;
	}
	else
	{
		return false;
	}
}

FString FName::ToString() const
{
	return FNamePool::GetDisplayString(Entry);
}

size_t FName::GetHash() const
{
	const size_t Bucket =
		static_cast<size_t>(Entry.ComparisonBucketIndex);
	const size_t Index =
		static_cast<size_t>(Entry.ComparisonIndex);
	
	// Index와 Bucket을 하나의 값으로 해싱.
	// 참고자료: 비슷한 목표를 가진 boost::container_hash의 hash_combine 함수의 이전 버전 구현
	// https://www.boost.org/doc/libs/latest/libs/container_hash/doc/html/hash.html#notes_hash_combine
	
	// 0x9e3779b9u = 1 / φ 로 황금비를 가리킴. zero trap 방지용
	// (Bucket << 6) + (Bucket >> 2)의 경우, Bucket의 비트가 여러 부분의 해싱 연산에 영향이 갈 수 있도록 위치를 옮기는 것
	const size_t MixedIndex = Index + 0x9e3779b9u + (Bucket << 6) + (Bucket >> 2);

	return Bucket ^ MixedIndex;

	// Note: 위 코드는 32비트 호환성을 위한건데, 그냥 64비트 강제할거면 엄청 간단하게 아래로 해도 됨...
	// 대신 아래 코드는 32비트 프로그램으로 빌드하면 절대 안됨
	//return (Bucket << 32) ^ Index;
}
