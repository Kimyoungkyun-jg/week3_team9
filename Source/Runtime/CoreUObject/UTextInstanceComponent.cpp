#include "UTextInstanceComponent.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "Runtime/Rendering/FRenderer.h"
#include "Runtime/Engine/UScene.h"
#include "Runtime/Rendering/ShaderConstants.h"
#include "UClass.h"
#include <algorithm>
#include <limits>
#include <windows.h>

IMPLEMENT_UCLASS(UTextInstanceComponent, UInstancePrimitiveComponent)

void UTextInstanceComponent::Register(UScene& InScene)
{
	FRenderResourceLibrary* Resources = InScene.GetRenderResourceLibrary();

	if (!Font) {
        Font = MakeShared<FFont>();
        FWString Path = GetExecutableDirectory() + L"/Fonts/MaplestoryBold.json";
        Font->Deserialize(Path);
	}

	if (!GetMesh()) {
		SetMesh(Resources ? Resources->GetMesh(EMeshID::Rect) : nullptr);
	}
	if (!GetMaterial()) {
		SetMaterial(Resources ? Resources->GetMaterial(EMaterialID::Instance_Text) : nullptr);
	}

	RebuildTextMesh();

	Super::Register(InScene);

}

void UTextInstanceComponent::Update(float delta)
{
    RebuildTextMesh();
}

void UTextInstanceComponent::RebuildTextMesh() {
    TextInstances.clear();
    Width = 0;
    Height = 0;

    if (Text.empty() || !Font) return;

    // Phase 1 : 전체 Bound 값 계산

    // Calculate the visible text bounds first so the component origin can be
    // used as the billboard pivot for the whole string.
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();

    float maxY = std::numeric_limits<float>::lowest();
    float maxZ = std::numeric_limits<float>::lowest();
    
    // 현재 텍스트에 글리프가 단 하나라도 있는지 체크
    bool bHasVisibleGlyph = false;

    float prevAdvance = 0.0f;
    for (uint16 i = 0; i < Text.length(); ++i)
    {
        const FCharacterInfo& CharInfo = Font->GetCharInfo(Text.at(i));
        if (Text.at(i) == ' ' || Text.at(i) == '\t')
        {
            prevAdvance += CharInfo.advance;
            continue;
        }

        const float glyphLeft = CharInfo.planeLeft + prevAdvance;
        const float glyphRight = CharInfo.planeRight + prevAdvance;
        const float glyphTop = -CharInfo.planeTop;
        const float glyphBottom = -CharInfo.planeBottom;

        minY = std::min(minY, std::min(glyphLeft, glyphRight));
        minZ = std::min(minZ, std::min(glyphTop, glyphBottom));

        maxY = std::max(maxY, std::max(glyphLeft, glyphRight));
        maxZ = std::max(maxZ, std::max(glyphTop, glyphBottom));

        bHasVisibleGlyph = true;

        prevAdvance += CharInfo.advance;
    }

    // 글리프가 없다면 렌더링할 데이터가 없음
    if (!bHasVisibleGlyph) return;

    Width = maxY - minY;
    Height = maxZ - minZ;
    const float textCenterY = (minY + maxY) * 0.5f;
    const float textCenterZ = (minZ + maxZ) * 0.5f;

    // Phase 2: 실제 FInstanceData 계산

    prevAdvance = 0.0f;
    for (uint16 i = 0; i < Text.length(); ++i)
    {
        const FCharacterInfo& CharInfo = Font->GetCharInfo(Text.at(i));
        if (Text.at(i) == ' ' || Text.at(i) == '\t')
        {   // 공백일 경우 인스턴스를 생성하지 않고 위치만 누적
            prevAdvance += CharInfo.advance;
            continue;
        }
        // 원본과 동일하게 4개 정점 좌표 및 UV 계산
        FVertexData tv[4]{};
        tv[0].x = 0.0f;
        tv[0].y = CharInfo.planeLeft + prevAdvance;
        tv[0].z = -CharInfo.planeTop;
        tv[0].u = CharInfo.u;
        tv[0].v = CharInfo.v;
        
        tv[1].x = 0.0f;
        tv[1].y = CharInfo.planeRight + prevAdvance;
        tv[1].z = -CharInfo.planeTop;
        tv[1].u = CharInfo.u + CharInfo.width;
        tv[1].v = CharInfo.v;
        
        tv[2].x = 0.0f;
        tv[2].y = CharInfo.planeLeft + prevAdvance;
        tv[2].z = -CharInfo.planeBottom;
        tv[2].u = CharInfo.u;
        tv[2].v = CharInfo.v + CharInfo.height;
        
        tv[3].x = 0.0f;
        tv[3].y = CharInfo.planeRight + prevAdvance;
        tv[3].z = -CharInfo.planeBottom;
        tv[3].u = CharInfo.u + CharInfo.width;
        tv[3].v = CharInfo.v + CharInfo.height;
        

        float charWidth = tv[1].y - tv[0].y;
        float charHeight = tv[0].z - tv[2].z; // planeTop - planeBottom
        float centerY = (tv[0].y + tv[1].y) * 0.5f - textCenterY;
        float centerZ = (tv[0].z + tv[2].z) * 0.5f - textCenterZ;
       
        // 글자별 순수 로컬 변환 (크기 * 위치)
        FMatrix CharMatrix = FMatrix::MakeScale(FVector(1.0f, charWidth, charHeight))
            * FMatrix::MakeTranslation(FVector(0.0f, centerY, centerZ));

        FInstanceData Data{};
        Data.World = CharMatrix;
        Data.Color = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
        Data.Center = FVector(0.0f, 0.0f, 0.0f);
        Data.Size = FVector2(1.0f, 1.0f);
        Data.UVScale = FVector2(CharInfo.width, CharInfo.height);
        Data.UVOffset = FVector2(tv[0].u, tv[0].v);
       
        TextInstances.push_back(Data);
        prevAdvance += CharInfo.advance;
    }
}


void UTextInstanceComponent::Render(FRenderer& renderer, const FCamera& Camera, const bool& bHighlighted) 
{
	if (!GetMesh() || !GetMaterial() || TextInstances.empty()) {
		return;
	}

    FTransform Transform = GetGlobalTransform();

    TArray<FInstanceData> RenderInstances = TextInstances;
    for (auto& Instance : RenderInstances)
    {
        Instance.Center = Transform.Location;
        Instance.Size = FVector2{ Transform.Scale3D.Y, Transform.Scale3D.Z };
    }

    renderer.AddTextInstanceArray(RenderInstances, GetMesh()->MeshId, GetMaterial()->MaterialId);
}
