#include "FRenderPipeline.h"

void FRenderPipeline::Bind(ID3D11DeviceContext& Context) const
{
	Context.IASetInputLayout(InputLayout.Get());

	Context.VSSetShader(VertexShader.Get(), nullptr, 0);
	Context.PSSetShader(PixelShader.Get(), nullptr, 0);

	Context.RSSetState(RasterizerState.Get());
	Context.OMSetDepthStencilState(DepthStencilState.Get(), 0);
}
