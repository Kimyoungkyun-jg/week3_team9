#pragma once

#include "Runtime/Core/IntTypes.h"
#include "Runtime/Math/FVector.h"
#include <d3d11.h>
#include <iterator>

enum class EVertexLayout : uint8
{
	None,
	PositionColor,
};

inline constexpr D3D11_INPUT_ELEMENT_DESC PositionColorElements[] = {
	{
		"POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0
	},
	{
		"COLOR", 0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0, D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA, 0
	},
};

struct FVertexLayoutDesc
{
	const D3D11_INPUT_ELEMENT_DESC* InputElements;
	uint32 InputElementCount;
};

inline FVertexLayoutDesc GetVertexLayoutDesc(EVertexLayout Layout)
{
	switch (Layout)
	{
	case EVertexLayout::None:
		return { {}, 0 };
	case EVertexLayout::PositionColor:
		return { PositionColorElements, std::size(PositionColorElements) };
	}

	return { nullptr, 0 };
};

struct FVertexPositionColor
{
	FVector Position;
	FVector Color;
};
