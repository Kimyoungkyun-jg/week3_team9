#pragma once

#include "UInstancePrimitiveComponent.h"
#include "Runtime/Rendering/FFont.h"
#include "Runtime/CoreUObject/UInstancePrimitiveComponent.h"

class UTextInstanceComponent : public UInstancePrimitiveComponent
{
	GENERATED_BODY()
	DECLARE_UCLASS(UTextInstanceComponent, UInstancePrimitiveComponent)

public:
	void Register(UScene& InScene) override;
	void Update(float delta) override;

	void SetText(const FWString& InText) {
		Text = InText;
		RebuildTextMesh();
	}

	[[nodiscard]] const FWString& GetText() const { return Text; }
	void SetFont(TSharedPtr<FFont> InFont) { Font = InFont; }

	void RebuildTextMesh();

	void Render(FRenderer& renderer, const FCamera& Camera, const bool& bHighlighted) override;

	virtual EEngineShowFlags GetShowFlag() const { return EEngineShowFlags::SF_BillboardText; }

private:
	TSharedPtr<FFont> Font;
	FWString Text = L"Hello Jungle World!";

	int32 Width = 0;
	int32 Height = 0;

	TArray<FInstanceData> Instances;

};
