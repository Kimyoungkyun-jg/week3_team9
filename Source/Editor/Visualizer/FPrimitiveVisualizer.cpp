#include "FPrimitiveVisualizer.h"

#include "Runtime/CoreUObject/UPrimitiveComponent.h"
#include "Runtime/Geometry/FAxisAlignedBoundingBox.h"
#include "Runtime/Engine/FRenderView.h"
#include "Runtime/Rendering/FMesh.h"
#include "Runtime/Math/FMatrix.h"

void FPrimitiveVisualizer::Draw(
	const UPrimitiveComponent& Component,
	FRenderView& RenderView,
	const FCamera& Camera,
	const FVector4& Color
) const
{
	if (Component.IsA<UPrimitiveComponent>() == false) { return; }

	const FMesh& Mesh = *Component.GetMesh();
	const FMatrix ModelMatrix = Component.GetRenderMatrix(Camera);
	FAxisAlignedBoundingBox AABB{ Mesh, ModelMatrix };
	RenderView.RenderBoxMinMax(AABB.Min, AABB.Max, Color);
}
