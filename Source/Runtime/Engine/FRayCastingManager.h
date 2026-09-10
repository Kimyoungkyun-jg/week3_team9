#pragma once

#include "Runtime/Math/FVector.h"
#include "Runtime/Core/TArray.h"
#include "Runtime/Math/FVector2.h"
#include "Runtime/Engine/FCamera.h"

class UPrimitiveComponent;
class FMesh;

struct FRay
{
	FVector Origin;
	FVector Direction;
};

namespace FRayCastingManager
{
    FRay CreateRayFromScreenPosition(const FCamera& Camera, const FVector2& MousePosition, const FVector2& ViewportSize);
    bool RayIntersectsMeshes(const FRay& Ray, const TArray<UPrimitiveComponent*>& Components, UPrimitiveComponent*& HitComponent, FVector& OutImpactPoint);
    bool RayIntersectsMesh(const FRay& Ray, const FMesh& Mesh, const FMatrix& ModelMatrix, float& OutDistance, FVector& OutImpactPoint);
    bool RayIntersectsTriangle(const FRay& Ray, const FVector& A, const FVector& B, const FVector& C, float& OutT);
};
