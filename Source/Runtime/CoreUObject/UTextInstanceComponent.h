#pragma once

#include "UInstancePrimitiveComponent.h"
#include "Runtime/Rendering/FFont.h"

class UTextInstanceComponent : public UInstancePrimitiveComponent
{
	GENERATED_BODY()
	DECLARE_UCLASS(UTextInstanceComponent, UInstancePrimitiveComponent)

public:
	void Register(UScene& InScene) override;

	void SetText(const FString& InText) {
		Text = InText;
		RebuildTextMesh();
	}

	[[nodiscard]] const FString& GetText() const { return Text; }
	void SetFont(TSharedPtr<FFont> InFont) { Font = InFont; }

	void RebuildTextMesh();

	void Render(FRenderer& renderer, const FCamera& Camera, const bool& bHighlighted) override;

private:
	TSharedPtr<FFont> Font;
	FString Text = "Hello Jungle World!";

};
