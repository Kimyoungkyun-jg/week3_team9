#include "FSpotlightVisualizer.h"

#include "Runtime/CoreUObject/UPrimitiveComponent.h"
#include "Runtime/CoreUObject/USpotLightComponent.h"
#include "Runtime/Geometry/FAxisAlignedBoundingBox.h"
#include "Runtime/Engine/FRenderView.h"
#include "Runtime/Rendering/FMesh.h"
#include "Runtime/Math/FMatrix.h"

void FSpotlightVisualizer::Draw(
	const UPrimitiveComponent& Component,
	FRenderView& RenderView,
	const FCamera& Camera
) const
{
    if (Component.IsA<USpotLightComponent>() == false) { return; }

    auto& Mesh = *Component.GetMesh();
    const FMatrix ModelMatrix = Component.GetRenderMatrix(Camera);

    const auto& Positions = Mesh.GetPositions();
    const auto& Indices = Mesh.GetIndices();

    const FVector4 WireColor{ 1.0f, 1.0f, 0.0f, 1.0f }; // 노란색 선

    // 메쉬의 삼각형 인덱스를 순회하며 모서리 선 그리기
    for (size_t i = 0; i + 2 < Indices.size(); i += 3)
    {
        FVector A = ModelMatrix.TransformPointRow(Positions[Indices[i]]);
        FVector B = ModelMatrix.TransformPointRow(Positions[Indices[i + 1]]);
        FVector C = ModelMatrix.TransformPointRow(Positions[Indices[i + 2]]);

        RenderView.RenderLine(A, B, WireColor);
        RenderView.RenderLine(B, C, WireColor);
        RenderView.RenderLine(C, A, WireColor);
    }
}
