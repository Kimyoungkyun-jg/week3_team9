#include "UTextComponent.h"

void UTextComponent::RebuildTextMesh()
{
	TArray<FTextVertex> Vertices;

	// TODO: PlaneGenerator 만들어야 함.
	FTextVertex plane[4] =
	{
		{ { -0.5f, 0.5f, 0.0f }, 0.0f, 0.0f },
		{ { 0.5f, 0.5f, 0.0f }, 0.0f, 0.0f },
		{ { -0.5f, -0.5f, 0.0f }, 0.0f, 0.0f },
		{ { -0.5f, 0.5f, 0.0f }, 0.0f, 0.0f }
	};

	TArray<uint32> IndexSet = { 0, 1, 2, 1, 3, 2 };
	TArray<uint32> Indices;
	const float size = 0.5f;
	const uint16 indexStride = 6;	// 플레인 메시 하나에 6개의 인덱스를 갖고 있음.
	for (uint16 i = 0; i < Text.length(); ++i) 
	{
		for (uint16 j = 0; j < 4; ++j)
		{	// ranged-for 로 수정?
			FTextVertex tv;
			float sizeAmount = size * i;
			tv.Pos = plane[i].Pos + FVector(sizeAmount, sizeAmount, sizeAmount);
			
			float width = (j % 2 == 0) ? Font->GetCharInfo(Text.at(i)).width : 0.0f;
			float height = (j % 2 != 0) ? Font->GetCharInfo(Text.at(i)).height : 0.0f;
			tv.u = Font->GetCharInfo(Text.at(i)).u + width;
			tv.v = Font->GetCharInfo(Text.at(i)).u + height;
			Vertices.push_back(tv);
		}

		for (uint32 index : IndexSet)
		{
			Indices.push_back(index * indexStride);
		}
	}

	// 메시 빌드 추가하기
}
