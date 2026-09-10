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
	bool InitializeGridConstantBuffers();

	template <typename T = FObjectConstants>
	void Draw(const FMesh& Mesh, const FMaterial& Material, const T& ObjectConstants, ID3D11Buffer* Buffer)
	{
		UpdateConstants<T>(ObjectConstants, Buffer);

		if (Mesh.GetVertexLayout() != Material.GetVertexLayout())
		{
			return;
		}

		const auto& Pipeline = Material.Pipeline;

		Pipeline->Bind(*Context.Get());
		Material.BindResources(*Context.Get());
		Mesh.BindResources(*Context.Get());

		Context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

		if (Mesh.HasIndices())
		{
			Context->DrawIndexed(Mesh.IndexCount, 0, 0);
		}
		else
		{
			Context->Draw(Mesh.VertexCount, 0);
		}
	}

	template <typename T>
	void UpdateConstants(const T& Constants, ID3D11Buffer* Buffer)
	{
		static const FMatrix UnrealClipToD3DClip
		{
			FVector{ 0.0f, 0.0f, 1.0f },
			FVector{ 1.0f, 0.0f, 0.0f },
			FVector{ 0.0f, 1.0f, 0.0f },
			FVector{ 0.0f, 0.0f, 0.0f }
		};

		// 언리얼 Clip -> D3D Clip 좌표 변환
		T ShaderConstants = Constants;
		ShaderConstants.MVP *= UnrealClipToD3DClip;

		D3D11_MAPPED_SUBRESOURCE MappedResource{};
		Context->Map(Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		memcpy(MappedResource.pData, &ShaderConstants, sizeof(ShaderConstants));
		Context->Unmap(Buffer, 0);

		Context->VSSetConstantBuffers(0, 1, &Buffer);
		Context->PSSetConstantBuffers(0, 1, &Buffer);
	}

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
