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

	if (!Font) {
		Font = MakeShared<FFont>();
		Font->Initialize(16);
	}

	if (!GetMesh()) {
		SetMesh(Resources ? Resources->GetMesh("Text") : nullptr);
	}
	if (!GetMaterial()) {
		SetMaterial(Resources ? Resources->GetMaterial(EMaterialID::Instance_Text) : nullptr);
	}

	RebuildTextMesh();

	Super::Register(InScene);

}

void UTextInstanceComponent::RebuildTextMesh()
{
	if (Text.empty() || !Font) return;

	// 첫 글자가 128 이상이면 한글
	const bool bIsKorean = (static_cast<unsigned char>(Text[0]) >= 128);

	if (bIsKorean) {
		SetTextureByName("koreanatlas");
	}
	else {
		SetTextureByName("englishatlas");
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

	Instances.clear();

	const float size = bIsKorean ? 1.0f : 0.55f;
	const uint32 CharCount = bIsKorean ? static_cast<uint32>(WideText.length()) : static_cast<uint32>(Text.length());
	const float totalWidth = (CharCount > 0) ? (CharCount - 1) * size : 0.0f;
	const float startOffset = -totalWidth * 0.5f;

	const FMatrix ComponentWorld = GetGlobalTransform().ToMatrix();

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

		float sizeAmount = startOffset + size * i;
		FMatrix LocalCharMatrix = FMatrix::MakeTranslation(FVector(0.0f, sizeAmount, 0.0f));

		FInstanceData Data;
		Data.Word = LocalCharMatrix * ComponentWorld;
		Data.Color = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		Data.UV = FVector2(CharInfo.width, CharInfo.height);
		Data.UVOffset = FVector2(CharInfo.u, CharInfo.v);

		Instances.push_back(Data);
	}
}

void UTextInstanceComponent::Render(FRenderer& renderer, const FCamera& Camera, const bool& bHighlighted)
{
	if (!GetMesh() || !GetMaterial() || Instances.empty()) {
		return;
	}

	// 인스턴스 데이터를 렌더러에 전달
	renderer.AddTextInstanceArray(Instances);
}
