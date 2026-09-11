#pragma once

#include "UPrimitiveComponent.h"
#include "Runtime/Rendering/FFont.h"
#include "Runtime/Rendering/FMesh.h"

struct FTextVertex
{
    FVector Pos;
    float u, v;
};


class UTextComponent :
    public UPrimitiveComponent
{
    GENERATED_BODY()
    DECLARE_UCLASS(UTextComponent, UPrimitiveComponent)

public:
    void SetText(const FString& InText) { Text = InText; RebuildTextMesh(); }
    void SetFont(TSharedPtr<FFont> InFont) { Font = InFont; }

    void RebuildTextMesh();
private:
    TSharedPtr<FFont> Font;
    FString Text;

    FMeshDesc MeshData;
};

