#include "FFont.h"
#include "Runtime/Core/IntTypes.h"

void FFont::Initialize(float InNumberOfLine)
{
	// 16x16 코드페이지 437 기준
	float uvSize = 1.0f / InNumberOfLine;
	for (uint16 i = 0; i < 256; ++i)
	{
		uint16 col = i % 16;
		uint16 row = i / 16;

		FCharacterInfo ci;
		ci.u = col * uvSize;
		ci.v = row * uvSize;
		ci.width = uvSize;
		ci.height = uvSize;

		CharInfoMap[static_cast<char>(i)] = ci;
	}
}

const FCharacterInfo& FFont::GetCharInfo(char InCharacter) const
{
	auto it = CharInfoMap.find(InCharacter);
	if (it != CharInfoMap.end())
	{
		return it->second;
	}

	auto fallbackIt = CharInfoMap.find('?');
	if (fallbackIt != CharInfoMap.end())
	{
		return fallbackIt->second;
	}
		
	static const FCharacterInfo defaultInfo{};
	return defaultInfo;
}
