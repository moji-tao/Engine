#pragma once
#include "EngineRuntime/include/Function/Render/DirectX/D3D12HeapSlotAllocator.h"

namespace Engine
{
	class D3D12Device;

	class D3D12View
	{
	public:
		D3D12View(D3D12Device* InDevice, D3D12_DESCRIPTOR_HEAP_TYPE InType, ID3D12Resource* InResource);

		virtual ~D3D12View();

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const;

	private:
		void Destroy();

	protected:
		D3D12Device* Device;

		D3D12HeapSlotAllocator* HeapSlotAllocator = nullptr;

		ID3D12Resource* Resource = nullptr;

		D3D12HeapSlotAllocator::HeapSlot HeapSlot;

		D3D12_DESCRIPTOR_HEAP_TYPE Type;
	};

	class D3D12ShaderResourceView : public D3D12View
	{
	public:
		D3D12ShaderResourceView(D3D12Device* InDevice, const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc, ID3D12Resource* InResource);

		virtual ~D3D12ShaderResourceView();

	protected:
		void CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc);
	};

	class D3D12RenderTargetView : public D3D12View
	{
	public:
		D3D12RenderTargetView(D3D12Device* InDevice, const D3D12_RENDER_TARGET_VIEW_DESC& Desc, ID3D12Resource* InResource);

		virtual ~D3D12RenderTargetView();

	protected:
		void CreateRenderTargetView(const D3D12_RENDER_TARGET_VIEW_DESC& Desc);
	};

	class D3D12DepthStencilView : public D3D12View
	{
	public:
		D3D12DepthStencilView(D3D12Device* InDevice, const D3D12_DEPTH_STENCIL_VIEW_DESC& Desc, ID3D12Resource* InResource);

		virtual ~D3D12DepthStencilView();

	protected:
		void CreateDepthStencilView(const D3D12_DEPTH_STENCIL_VIEW_DESC& Desc);
	};

	class D3D12UnorderedAccessView : public D3D12View
	{
	public:
		D3D12UnorderedAccessView(D3D12Device* InDevice, const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc, ID3D12Resource* InResource);

		virtual ~D3D12UnorderedAccessView();

	protected:
		void CreateUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc);
	};
}
