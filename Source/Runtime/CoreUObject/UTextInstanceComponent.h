#pragma once

#include "UInstancePrimitiveComponent.h"
#include "Runtime/Rendering/FFont.h"

class UTextInstanceComponent : public UInstancePrimitiveComponent
{
	GENERATED_BODY()
	DECLARE_UCLASS(UTextInstanceComponent, UInstancePrimitiveComponent)

public:
	void Register(UScene& InScene) override;

	void SetText(const FWString& InText) {
		Text = InText;
		RebuildTextMesh();
	}

	[[nodiscard]] const FWString& GetText() const { return Text; }
	void SetFont(TSharedPtr<FFont> InFont) { Font = InFont; }

	void RebuildTextMesh();

	void Render(FRenderer& renderer, const FCamera& Camera, const bool& bHighlighted) override;

private:
	TSharedPtr<FFont> Font;
	FWString Text = L"Hello Jungle World!";

};
