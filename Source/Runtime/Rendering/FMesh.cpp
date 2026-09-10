#include "FMesh.h"

#include <d3d11.h>
#include <wrl/client.h>

void FMesh::BindResources(ID3D11DeviceContext& Context) const
{
	constexpr UINT Offset = 0;

	Context.IASetPrimitiveTopology(Topology);
	Context.IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &VertexStride, &Offset);
	Context.IASetIndexBuffer(IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}
