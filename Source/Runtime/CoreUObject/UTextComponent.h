#pragma once

#include "Runtime/Rendering/FFont.h"
#include "Runtime/Rendering/FMesh.h"
#include "UBillBoardComp.h"

class UTextComponent : public UBillBoardComp {
  GENERATED_BODY()
  DECLARE_UCLASS(UTextComponent, UBillBoardComp)

public:
  void SetText(const FString &InText) {
    Text = InText;
    RebuildTextMesh();
  }
  [[nodiscard]] const FString &GetText() const { return Text; }
  void SetFont(TSharedPtr<FFont> InFont) { Font = InFont; }

  void Register(UScene& Scene) override;
  void RebuildTextMesh();

private:
  TSharedPtr<FFont> Font;
  FString Text = "Hello Jungle World!";

  FMeshDesc MeshData;
};
