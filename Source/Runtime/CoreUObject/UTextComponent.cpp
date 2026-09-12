#include "UTextComponent.h"
#include "UClass.h"
#include "Runtime/Engine/UScene.h"

IMPLEMENT_UCLASS(UTextComponent, UPrimitiveComponent)
UCLASS_META(UTextComponent, DisplayName, "Text")
UCLASS_META(UTextComponent, MeshName, "Text")

void UTextComponent::OnRegister(UScene& Scene)
{
	Super::OnRegister(Scene);

	SetMesh(Scene.GetRenderResourceLibrary().GetMesh("Text"));
}

void UTextComponent::RebuildTextMesh()
{
	//TArray<FTextVertex> Vertices;
	//TArray<uint32> Indices;

	//// TODO: PlaneGenerator 만들어야 함.
	//FTextVertex plane[4] =
	//{
	//	{ { -0.5f, 0.5f, 0.0f }, 0.0f, 0.0f },
	//	{ { 0.5f, 0.5f, 0.0f }, 0.0f, 0.0f },
	//	{ { -0.5f, -0.5f, 0.0f }, 0.0f, 0.0f },
	//	{ { 0.5f, -0.5f, 0.0f }, 0.0f, 0.0f }
	//};
	//TArray<uint32> IndexSet = { 0, 1, 2, 1, 3, 2 };
	//
	//const float size = 0.5f;
	//for (uint16 i = 0; i < Text.length(); ++i) 
	//{
	//	const FCharacterInfo& CharInfo = Font->GetCharInfo(Text.at(i));
	//	for (uint16 j = 0; j < 4; ++j)
	//	{	// ranged-for 로 수정?
	//		FTextVertex tv;
	//		float sizeAmount = size * i;
	//		tv.Pos = plane[j].Pos + FVector(sizeAmount, 0.0f, 0.0f);

	//		bool bIsRight = (j == 1) || (j == 3);
	//		bool bIsBottom = (j == 2) || (j == 3);
	//		
	//		float width = (bIsRight) ? CharInfo.width : 0.0f;
	//		float height = (bIsBottom) ? CharInfo.height : 0.0f;
	//		tv.u = CharInfo.u + width;
	//		tv.v = CharInfo.v + height;
	//		Vertices.push_back(tv);
	//	}

	//	uint32 VertexOffset = i * 4;
	//	for (uint32 index : IndexSet)
	//	{
	//		Indices.push_back(index + VertexOffset);
	//	}
	//}

	//// 메시 빌드 추가하기
	//MeshData.VertexData = Vertices.data();
	//MeshData.VertexDataSize = static_cast<uint32>(sizeof(FVertexData) * Vertices.size());
	//MeshData.VertexStride = sizeof(FVertexData);
	//MeshData.VertexCount = static_cast<uint32>(Vertices.size());
	//MeshData.IndexData = Indices.data();
	//MeshData.IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size());
	//MeshData.IndexCount = static_cast<uint32>(Indices.size());
}
