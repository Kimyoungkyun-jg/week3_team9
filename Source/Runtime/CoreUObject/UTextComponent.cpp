#include "UTextComponent.h"
#include "Runtime/Engine/UScene.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "Runtime/Rendering/FRenderer.h"
#include "UClass.h"

IMPLEMENT_UCLASS(UTextComponent, UBillBoardComp)
UCLASS_META(UTextComponent, DisplayName, "Text")
UCLASS_META(UTextComponent, MeshName, "Text")

void UTextComponent::OnRegister(UScene &Scene) {
  Super::OnRegister(Scene);

  if (!Font) { //폰트 초기화
    Font = MakeShared<FFont>();
    Font->Initialize(16);
  }

  SetMaterial(Scene.GetRenderResourceLibrary().GetTextMaterial());
  RebuildTextMesh();
}

void UTextComponent::RebuildTextMesh() {
    if (Text.empty()) return;
    
    //첫 글자가 128 이상이면 한글, 미만이면 영어
    const bool bIsKorean = (static_cast<unsigned char>(Text[0]) >= 128);

    if (bIsKorean) {
        SetTexture("koreanatlas");
    }
    else {
        SetTexture("englishatlas");
    }

    FWString WideText;
    if (bIsKorean) {
        FString CleanText = Text.c_str();
        int len = MultiByteToWideChar(CP_UTF8, 0, CleanText.data(), static_cast<int>(CleanText.size()), nullptr, 0);
        if (len > 0) {
            WideText.resize(len);
            MultiByteToWideChar(CP_UTF8, 0, CleanText.data(), static_cast<int>(CleanText.size()), &WideText[0], len);
        }
        while (!WideText.empty() && (WideText.back() == L'\0' || WideText.back() == L'\r' || WideText.back() == L'\n')) {
            WideText.pop_back();
        }
    }

    TArray<FVertexData> Vertices;
    TArray<uint32> Indices;

    FTextVertex plane[4] = { { { 0.0f, -0.5f, 0.5f }, 0.0f, 0.0f },
                            { { 0.0f, 0.5f, 0.5f }, 0.0f, 0.0f },
                            { { 0.0f, -0.5f, -0.5f }, 0.0f, 0.0f },
                            { { 0.0f, 0.5f, -0.5f }, 0.0f, 0.0f } };
    TArray<uint32> IndexSet = { 0, 1, 2, 1, 3, 2, 0, 2, 1, 1, 2, 3 };

    // 텍스트 가운데 정렬
    const float size = bIsKorean ? 1.0f : 0.55f;
    const uint32 CharCount = bIsKorean ? static_cast<uint32>(WideText.length()) : static_cast<uint32>(Text.length());
    const float totalWidth = (CharCount > 0) ? (CharCount - 1) * size : 0.0f;
    const float startOffset = -totalWidth * 0.5f;

    for (uint32 i = 0; i < CharCount; ++i) {
        if (bIsKorean && (WideText[i] == L' ' || WideText[i] == L'\t')) {
            continue;
        }
        if (!bIsKorean && (Text[i] == ' ' || Text[i] == '\t')) {
            continue;
        }

        FCharacterInfo CharInfo;
        if (bIsKorean) {
            CharInfo = Font->GetKrCharInfo(WideText[i]);
        }
        else {
            CharInfo = Font->GetEngCharInfo(Text[i]);
        }

        uint32 VertexOffset = static_cast<uint32>(Vertices.size());
        for (uint32 j = 0; j < 4; ++j) {
            FVertexData tv;
            float sizeAmount = startOffset + size * i;
            tv.x = plane[j].Pos.X;
            tv.y = plane[j].Pos.Y + sizeAmount;
            tv.z = plane[j].Pos.Z;

            bool bIsRight = (j == 1) || (j == 3);
            bool bIsBottom = (j == 2) || (j == 3);

            float width = (bIsRight) ? CharInfo.width : 0.0f;
            float height = (bIsBottom) ? CharInfo.height : 0.0f;
            tv.u = CharInfo.u + width;
            tv.v = CharInfo.v + height;
            Vertices.push_back(tv);
        }

        for (uint32 index : IndexSet) {
            Indices.push_back(index + VertexOffset);
        }
    }



    FMeshDesc MeshData{
        .VertexData = Vertices.data(),
        .VertexDataSize =
            static_cast<uint32>(sizeof(FVertexData) * Vertices.size()),
        .VertexStride = sizeof(FVertexData),
        .VertexCount = static_cast<uint32>(Vertices.size()),
        .IndexData = Indices.data(),
        .IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
        .IndexCount = static_cast<uint32>(Indices.size())};

    FRenderer* Renderer = FRenderResourceLibrary::Get().GetRenderer();
    if (!Renderer) {
        return;
    }

    // 기존 메쉬 버퍼 직접 갱신
    auto Mesh = GetMesh();
    if (Mesh) {
        Mesh->UpdateBuffers(Renderer->GetDevice(), Renderer->GetContext(), MeshData);
    } else {
        // 신규 메쉬 생성
        SetMesh(Renderer->CreateDynamicMesh(MeshData));
    }
}


