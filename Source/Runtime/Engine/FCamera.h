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
	FVector UpVector{ 0.0f,0.0f,1.0f };

	// TODO: 캐시 가능, 캐시하려면 세터를 넣어야 함
	[[nodiscard]] FMatrix CreateViewProjectionMatrix() const;
	[[nodiscard]] FMatrix GetRotationMatrix() const;
	[[nodiscard]] FMatrix GetProjectionMatrix() const;


	// Move(), Rotate(), Zoom() 등 추가 가능
};

inline FMatrix FCamera::GetRotationMatrix() const
{
	// 카메라 회전 행렬 반환
	return FMatrix::MakeRotation(FVector(0.0f, Pitch, Yaw));
}

inline FMatrix FCamera::GetProjectionMatrix() const
{
	// 카메라 투영 행렬 반환
	return Projection.CreateProjectionMatrix();
}

inline FMatrix FCamera::CreateViewProjectionMatrix() const
{
	const FMatrix InverseRotationMatrix = GetRotationMatrix().Transpose();

	// 뷰 행렬 계산
	const FMatrix ViewMatrix = FMatrix::MakeTranslation(-Position) * InverseRotationMatrix;

	const FMatrix ProjectionMatrix = GetProjectionMatrix();

	return ViewMatrix * ProjectionMatrix;
}
