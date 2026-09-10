#pragma once

#include "Vertices.h"
#include <d3d11.h>
#include <wrl/client.h>

class FRenderPipeline final
{
	friend class FRenderer;

public:
	[[nodiscard]] EVertexLayout GetVertexLayout() const { return VertexLayout; }

private:
	FRenderPipeline() = default;

	void Bind(ID3D11DeviceContext& Context) const;

	EVertexLayout VertexLayout = EVertexLayout::None;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
	// Blend Mode 등 추가 가능
};
