#pragma once

#include "Runtime/Core/TMap.h"

struct FCharacterInfo
{
	float u;
	float v;
	float width;
	float height;
};

class FFont
{
public:
	void Initialize(float InNumberOfLine);

	const FCharacterInfo& GetEngCharInfo(char InCharacter) const;
	const FCharacterInfo& GetKrCharInfo(wchar_t InCharacter) const;
private:


	TMap<char, FCharacterInfo> EngCharInfoMap;
	TMap<wchar_t, FCharacterInfo> KrCharInfoMap;
};