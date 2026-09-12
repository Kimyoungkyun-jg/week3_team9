#include "FArchive.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

FArchive::FArchive()
	: Object()
{
}

FArchive::FArchive(const nlohmann::json& InObject)
	: Object(InObject)
{
}

int32 FArchive::GetInt32(const FString& Key) const
{
	return Object.at(Key).get<int32>();
}

void FArchive::SetInt32(const FString& Key, int32 Value)
{
	Object[Key] = Value;
}

float FArchive::GetFloat(const FString& Key) const
{
	return Object.at(Key).get<float>();
}

void FArchive::SetFloat(const FString& Key, float Value)
{
	Object[Key] = Value;
}

uint32 FArchive::GetUInt32(const FString& Key) const
{
	return Object.at(Key).get<uint32>();
}

void FArchive::SetUInt32(const FString& Key, uint32 Value)
{
	Object[Key] = Value;
}

double FArchive::GetDouble(const FString& Key) const
{
	return Object.at(Key).get<double>();
}

void FArchive::SetDouble(const FString& Key, double Value)
{
	Object[Key] = Value;
}

bool FArchive::GetBool(const FString& Key) const
{
	return Object.at(Key).get<bool>();
}

void FArchive::SetBool(const FString& Key, bool Value)
{
	Object[Key] = Value;
}

FString FArchive::GetString(const FString& Key) const
{
	return Object.at(Key).get<FString>();
}

void FArchive::SetString(const FString& Key, const FString& Value)
{
	Object[Key] = Value;
}

TArray<const FArchive> FArchive::GetObjectArray(const FString& Key) const
{
	TArray<const FArchive> Array;

	for (const auto& Item : Object.at(Key))
	{
		const FArchive ItemArchive{ Item };
		Array.push_back(ItemArchive);
	}

	return Array;
}

void FArchive::SetObjectArray(const FString& Key, const TArray<const FArchive>& Value)
{
	Object[Key] = nlohmann::json::array();

	for (const auto& Item : Value)
	{
		Object.at(Key).push_back(Item.GetJSON());
	}
}

FArchive FArchive::GetObject(const FString& Key) const
{
	return FArchive{ Object.at(Key) };
}

void FArchive::SetObject(const FString& Key, const FArchive& Archive)
{
	Object[Key] = Archive.GetJSON();
}
