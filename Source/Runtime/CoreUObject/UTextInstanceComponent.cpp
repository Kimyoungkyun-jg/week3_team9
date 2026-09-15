#include "UTextInstanceComponent.h"
#include "Runtime/Rendering/FRenderResourceLibrary.h"
#include "Runtime/Rendering/FRenderer.h"
#include "Runtime/Engine/UScene.h"
#include "Runtime/Rendering/ShaderConstants.h"
#include "UClass.h"
#include <windows.h>

IMPLEMENT_UCLASS(UTextInstanceComponent, UInstancePrimitiveComponent)

void UTextInstanceComponent::Register(UScene& InScene)
{
	FRenderResourceLibrary* Resources = InScene.GetRenderResourceLibrary();

    SetFont();

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

void UTextInstanceComponent::SetFont()
{
    if (!Font) {
        Font = MakeShared<FFont>();
        FWString Path = GetExecutableDirectory() + L"/Fonts/MaplestoryBold.json";
        Font->Deserialize(Path);
    }
}

void UTextInstanceComponent::RebuildTextMesh() {
    
    
    if (Text.empty() || !Font) return;
    Instances.clear();
    const FMatrix ComponentWorld = GetGlobalTransform().ToMatrix();
    float prevAdvance = 0.0f;
    UINT blankCnt = 0;
    for (uint16 i = 0; i < Text.length(); ++i)
    {
        const FCharacterInfo& CharInfo = Font->GetCharInfo(Text.at(i));
        if (Text.at(i) == ' ' || Text.at(i) == '\t')
        {   // 공백일 경우 인스턴스를 생성하지 않고 위치만 누적
            prevAdvance += CharInfo.advance;
            ++blankCnt;
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
        float centerY = (tv[0].y + tv[1].y) * 0.5f;
        float centerZ = (tv[0].z + tv[2].z) * 0.5f;
       
       
        // 글자별 순수 로컬 변환 (크기 * 위치)
        FMatrix CharMatrix = FMatrix::MakeScale(FVector(1.0f, charWidth, charHeight))
            * FMatrix::MakeTranslation(FVector(0.0f, centerY, centerZ));

        FInstanceData Data;
        Data.Word = CharMatrix;
        Data.Color = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
        Data.UV = FVector2(CharInfo.width, CharInfo.height); // UV 크기
        Data.UVOffset = FVector2(tv[0].u, tv[0].v);          // UV 시작점
       
        Instances.push_back(Data);
        prevAdvance += CharInfo.advance;
    }
}


void UTextInstanceComponent::Render(FRenderer& renderer, const FCamera& Camera, const bool& bHighlighted)
{
	if (!GetMesh() || !GetMaterial() || Instances.empty()) {
		return;
	}


    const FTransform G = GetGlobalTransform();
    FVector ToCameraXY = Camera.Position - G.Location;
    ToCameraXY.Z = 0.0f;
    FMatrix Rotation = FMatrix::GetIdentity();
    if (ToCameraXY.SizeSquared() >= 1e-8f)
    {

        const FVector Forward = -ToCameraXY / ToCameraXY.Size();
        const FVector WorldUp{ 0.0f, 0.0f, 1.0f };
        FVector Right = WorldUp.Cross(Forward);
        if (Right.SizeSquared() < 1e-8f) {
            Right = FVector{ 0.0f, 1.0f, 0.0f };
        }
        Right = Right / Right.Size();
        const FVector Up = Forward.Cross(Right);
        Rotation = FMatrix{ Forward, Right, Up, FVector{ 0.0f, 0.0f, 0.0f } };
    }
    // 최종 빌보드 월드 행렬
    const FMatrix BillboardWorld = FMatrix::MakeScale(G.Scale3D)
        * Rotation
        * FMatrix::MakeTranslation(G.Location);

    TArray<FInstanceData> RenderInstances = Instances;
    for (auto& Instance : RenderInstances)
    {
        Instance.Word = Instance.Word * BillboardWorld;
    }

    renderer.AddTextInstanceArray(RenderInstances, GetMesh()->MeshId, GetMaterial()->MaterialId);
}
