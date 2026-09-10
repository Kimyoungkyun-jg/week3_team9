#pragma once

#include "FMesh.h"
#include "FMaterial.h"
#include "FRenderPipeline.h"
#include "ShaderConstants.h"
#include "Runtime/Core/PointerTypes.h"
#include "Runtime/Math/FVector2.h"
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>

class FRenderer final
{
public:
	bool Initialize(HWND Window);
	void Shutdown();
	void BeginFrame();
	void SetViewportUV(FVector2 TopLeftUV, FVector2 LengthUV);
	void Draw(const FMesh& Mesh, const FMaterial& Material, const FObjectConstants& ObjectConstants);
	void DrawGrid(const FMesh& Mesh, const FMaterial& Material, const FGridConstants& GridConstants);
	void ClearDepth();
	void SwapBuffer();
	void OnWindowSize(UINT Width, UINT Height);
	
	[[nodiscard]]
	TSharedPtr<FMesh> CreateMesh(const FMeshDesc& Desc);
	[[nodiscard]]
	TSharedPtr<FMaterial> CreateMaterial(const FMaterialDesc& Desc);

	void GetDeviceAndContext_ImplDX11(ID3D11Device*& DeviceOut, ID3D11DeviceContext*& ContextOut);

private:
	bool InitializeDeviceAndSwapChain(HWND Window);
	bool InitializeBackBufferAndDepthStencil();
	bool InitializeConstantBuffers();

	//TODO : Constant 데이터를 T로 받으면 좋을거같은데, 일단 하나 만들었음
	bool InitializeGridConstantBuffers();
	void UpdateObjectConstants(const FObjectConstants& Constants);
	//TODO : Constant 데이터를 T로 받으면 좋을거같은데, 일단 하나 만들었음
	void UpdateGridConstants(const FGridConstants& Constants);

	[[nodiscard]]
	TSharedPtr<FRenderPipeline> FindOrCreateRenderPipeline(const FMaterialDesc& Desc);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
	D3D11_VIEWPORT Viewport{};

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> BackBufferRTV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;

	Microsoft::WRL::ComPtr<ID3D11Buffer> ObjectConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> GridConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> FrameConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> EditorViewPortRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> EditorViewPortSRV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTexture;

};
