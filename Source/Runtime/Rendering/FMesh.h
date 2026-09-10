#pragma once

#include "Vertices.h"
#include "Runtime/Core/IntTypes.h"
#include <d3d11.h>
#include <wrl/client.h>
#include "Runtime/Core/TArray.h"

class FRenderer;

class FMesh final
{
	friend class FRenderer;

public:
	[[nodiscard]] bool HasIndices() const { return IndexCount > 0; }
	[[nodiscard]] EVertexLayout GetVertexLayout() const { return VertexLayout; }
	[[nodiscard]] uint32 GetVertexCount() const { return VertexCount; }
	[[nodiscard]] uint32 GetIndexCount() const { return IndexCount; }
	[[nodiscard]] const TArray<FVector>& GetPositions() const { return Positions; }
	[[nodiscard]] const TArray<uint32>& GetIndices() const { return Indices; }

private:
	FMesh() = default;

	void BindResources(ID3D11DeviceContext& Context) const;

	EVertexLayout VertexLayout = EVertexLayout::None;

	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	uint32 VertexCount = 0u;
	uint32 VertexStride = 0u;

	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
	uint32 IndexCount = 0u;

	TArray<FVector> Positions;
	TArray<uint32> Indices;

	D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

struct FMeshDesc
{
	EVertexLayout VertexLayout = EVertexLayout::None;

	const void* VertexData = nullptr;
	uint32 VertexDataSize = 0u;
	uint32 VertexStride = 0u;
	uint32 VertexCount = 0u;

	const void* IndexData = nullptr;
	uint32 IndexDataSize = 0u;
	uint32 IndexCount = 0u;

	bool bIsLine = false;
	// Topology, Index Format 등 추가 가능
};
