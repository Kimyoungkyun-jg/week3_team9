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

	const FCharacterInfo& GetCharInfo(char InCharacter) const;
private:
	//FTexture	// 텍스처 아틀라스
	//FMeterial	// 폰트 머터리얼

	TMap<char, FCharacterInfo> CharInfoMap;
};