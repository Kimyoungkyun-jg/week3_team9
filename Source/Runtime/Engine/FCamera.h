#pragma once

#include "FCameraProjection.h"
#include "Runtime/Math/FVector.h"
#include "Runtime/Math/FMatrix.h"

struct FCamera
{
	FVector Position{ 0.0f, 0.0f, 0.0f };
	float Yaw = 0.0f;
	float Pitch = 0.0f;
	FCameraProjection Projection;

	// TODO: 캐시 가능, 캐시하려면 세터를 넣어야 함
	[[nodiscard]] FMatrix CreateViewProjectionMatrix() const;

	// Move(), Rotate(), Zoom() 등 추가 가능
};

inline FMatrix FCamera::CreateViewProjectionMatrix() const
{
	const FMatrix InverseRotationMatrix = FMatrix::MakeRotation(FVector(0.0f, Pitch, Yaw)).Transpose();

	// (R * T)^-1 = T^-1 * R^-1 = T^-1 * R^T
	const FMatrix ViewMatrix = FMatrix::MakeTranslation(-Position) * InverseRotationMatrix;

	const FMatrix ProjectionMatrix = Projection.CreateProjectionMatrix();

	return ViewMatrix * ProjectionMatrix;
}
