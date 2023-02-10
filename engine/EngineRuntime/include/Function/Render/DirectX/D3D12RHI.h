#pragma once
#include "EngineRuntime/include/Function/Render/RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Device.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Viewport.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Texture.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Buffer.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/TextureData.h"

namespace Engine
{
	class ImGuiDevice;
	class RenderPipeline;

	class D3D12RHI : public RHI
	{
	public:
		D3D12RHI(WindowSystem* windowSystem);

		virtual ~D3D12RHI() override;

	private:
		void Initialize(HWND WindowHandle, int WindowWidth, int WindowHeight);

		void Destroy();

	public:
		virtual void InitializeRenderPipeline(std::unique_ptr<RenderPipeline>& renderPipeline, std::unique_ptr<RenderResource>& renderResource) override;

		D3D12Device* GetDevice();

		D3D12Viewport* GetViewport();

		const D3D12ViewportInfo& GetViewportInfo();

		IDXGIFactory4* GetDxgiFactory();

		virtual void InitEditorUI(std::unique_ptr<ImGuiDevice>& editorUI, WindowUI* windowUI) override;

	public:
		virtual void FlushCommandQueue() override;

		virtual void ExecuteCommandLists() override;

		virtual void ResetCommandList() override;

		virtual void ResetCommandAllocator() override;

		virtual void Present() override;

		virtual void EndFrame() override;

		virtual void ResizeViewport(int width, int height) override;

		void TransitionResource(D3D12Resource* Resource, D3D12_RESOURCE_STATES StateAfter);

		void CopyResource(D3D12Resource* DstResource, D3D12Resource* SrcResource);

		void CopyBufferRegion(D3D12Resource* DstResource, uint64_t DstOffset, D3D12Resource* SrcResource, uint64_t SrcOffset, uint64_t Size);

		void CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION* Dst, UINT DstX, UINT DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION* Src, const D3D12_BOX* SrcBox);

		D3D12ConstantBufferRef CreateConstantBuffer(const void* Contents, uint32_t Size);

		D3D12StructuredBufferRef CreateStructuredBuffer(const void* Contents, uint32_t ElementSize, uint32_t ElementCount);

		D3D12RWStructuredBufferRef CreateRWStructuredBuffer(uint32_t ElementSize, uint32_t ElementCount);

		D3D12VertexBufferRef CreateVertexBuffer(const void* Contents, uint32_t Size);

		D3D12IndexBufferRef CreateIndexBuffer(const void* Contents, uint32_t Size);

		D3D12ReadBackBufferRef CreateReadBackBuffer(uint32_t Size);

		D3D12TextureRef CreateTexture(const D3D12TextureInfo& TextureInfo, uint32_t CreateFlags, Vector4 RTVClearValue = Vector4::ZERO);

		// Use D3DResource to create texture, texture will manage this D3DResource
		D3D12TextureRef CreateTexture(Microsoft::WRL::ComPtr<ID3D12Resource> D3DResource, D3D12TextureInfo& TextureInfo, uint32_t CreateFlags);

		void UploadCubeTextureData(D3D12TextureRef Texture, const std::array<const TextureData*, 6>& textureData);
		
		void UploadTextureData(D3D12TextureRef Texture, const TextureData* textureData);

		void SetVertexBuffer(const D3D12VertexBufferRef& VertexBuffer, UINT Offset, UINT Stride, UINT Size);

		void SetIndexBuffer(const D3D12IndexBufferRef& IndexBuffer, UINT Offset, DXGI_FORMAT Format, UINT Size);

	private:
		void CreateDefaultBuffer(uint32_t Size, uint32_t Alignment, D3D12_RESOURCE_FLAGS Flags, D3D12ResourceLocation& ResourceLocation);

		void CreateAndInitDefaultBuffer(const void* Contents, uint32_t Size, uint32_t Alignment, D3D12ResourceLocation& ResourceLocation);

		D3D12TextureRef CreateTextureResource(const D3D12TextureInfo& TextureInfo, uint32_t CreateFlags, Vector4 RTVClearValue);

		void CreateTextureView(D3D12TextureRef TextureRef, const D3D12TextureInfo& TextureInfo, uint32_t CreateFlags);

	private:
		void LogAdapters();

		void LogAdapterOutputs(IDXGIAdapter* adapter);

		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

		unsigned GetSupportMSAAQuality(DXGI_FORMAT BackBufferFormat);

	private:
		std::unique_ptr<D3D12Device> Device = nullptr;

		std::unique_ptr<D3D12Viewport> Viewport = nullptr;

		D3D12ViewportInfo ViewportInfo;

		Microsoft::WRL::ComPtr<IDXGIFactory4> DxgiFactory = nullptr;
	};
}
