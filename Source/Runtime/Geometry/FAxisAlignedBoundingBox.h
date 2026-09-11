#pragma once

#include "Runtime/Math/FVector.h"
#include "Runtime/Core/TArray.h"

#include <limits>

// Note: 언젠가는 OBB를 추가로 구현해야할지도
class FMesh;

struct FAxisAlignedBoundingBox
{
	FVector Min
	{
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
	};

	FVector Max
	{
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::min(),
	};

	FAxisAlignedBoundingBox() = default;

	FAxisAlignedBoundingBox(const FMesh& Mesh);
};