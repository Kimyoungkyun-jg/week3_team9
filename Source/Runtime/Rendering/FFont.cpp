#include "FFont.h"
#include "Runtime/Core/IntTypes.h"
#include <windows.h>

void FFont::Initialize(float InNumberOfLine)
{
	// 영문 코드페이지 기준
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

		EngCharInfoMap[static_cast<char>(i)] = ci;
	}

	// 한글 아틀라스 셀 크기 기준
	constexpr float krCellSize = 40.0f;
	constexpr float krAtlasSize = 2048.0f;
	constexpr float krUvStep = krCellSize / krAtlasSize;

	uint16 krIndex = 0;
	for (unsigned char b1 = 0xB0; b1 <= 0xC8; ++b1)
	{
		for (unsigned char b2 = 0xA1; b2 <= 0xFE; ++b2)
		{
			char mb[3] = { static_cast<char>(b1), static_cast<char>(b2), '\0' };
			wchar_t wc = 0;
			MultiByteToWideChar(949, 0, mb, 2, &wc, 1);

			uint16 col = krIndex % 51;
			uint16 row = krIndex / 51;

			FCharacterInfo ci;
			ci.u = col * krUvStep;
			ci.v = row * krUvStep;
			ci.width = krUvStep;
			ci.height = krUvStep;

			KrCharInfoMap[wc] = ci;
			++krIndex;
		}
	}

}

const FCharacterInfo& FFont::GetEngCharInfo(char InCharacter) const
{
	auto it = EngCharInfoMap.find(InCharacter);
	if (it != EngCharInfoMap.end())
	{
		return it->second;
	}

	auto fallbackIt = EngCharInfoMap.find('?');
	if (fallbackIt != EngCharInfoMap.end())
	{
		return fallbackIt->second;
	}
		
	static const FCharacterInfo defaultInfo{};
	return defaultInfo;
}

const FCharacterInfo& FFont::GetKrCharInfo(wchar_t InCharacter) const
{
	auto it = KrCharInfoMap.find(InCharacter);
	if (it != KrCharInfoMap.end())
	{
		return it->second;
	}

	static const FCharacterInfo defaultInfo{};
	return defaultInfo;
}

