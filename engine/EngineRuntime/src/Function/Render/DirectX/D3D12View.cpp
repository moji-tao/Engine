#include <cassert>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12View.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Device.h"

namespace Engine
{
	D3D12View::D3D12View(D3D12Device* InDevice, D3D12_DESCRIPTOR_HEAP_TYPE InType, ID3D12Resource* InResource)
		:Device(InDevice), Type(InType), Resource(InResource)
	{
		HeapSlotAllocator = Device->GetHeapSlotAllocator(Type);

		if(HeapSlotAllocator)
		{
			HeapSlot = HeapSlotAllocator->AllocateHeapSlot();
			assert(HeapSlot.Handle.ptr != 0);
		}
	}

	D3D12View::~D3D12View()
	{
		Destroy();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12View::GetDescriptorHandle() const
	{
		return HeapSlot.Handle;
	}

	void D3D12View::Destroy()
	{
		if(HeapSlotAllocator)
		{
			HeapSlotAllocator->FreeHeapSlot(HeapSlot);
		}
	}

	D3D12ShaderResourceView::D3D12ShaderResourceView(D3D12Device* InDevice, const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc, ID3D12Resource* InResource)
		:D3D12View(InDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, InResource)
	{
		CreateShaderResourceView(Desc);
	}

	D3D12ShaderResourceView::~D3D12ShaderResourceView()
	{ }

	void D3D12ShaderResourceView::CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc)
	{
		Device->GetNativeDevice()->CreateShaderResourceView(Resource, &Desc, HeapSlot.Handle);
	}

	D3D12RenderTargetView::D3D12RenderTargetView(D3D12Device* InDevice, const D3D12_RENDER_TARGET_VIEW_DESC& Desc, ID3D12Resource* InResource)
		:D3D12View(InDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, InResource)
	{
		CreateRenderTargetView(Desc);
	}

	D3D12RenderTargetView::~D3D12RenderTargetView()
	{ }

	void D3D12RenderTargetView::CreateRenderTargetView(const D3D12_RENDER_TARGET_VIEW_DESC& Desc)
	{
		Device->GetNativeDevice()->CreateRenderTargetView(Resource, &Desc, HeapSlot.Handle);
	}

	D3D12DepthStencilView::D3D12DepthStencilView(D3D12Device* InDevice, const D3D12_DEPTH_STENCIL_VIEW_DESC& Desc, ID3D12Resource* InResource)
		:D3D12View(InDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, InResource)
	{
		CreateDepthStencilView(Desc);
	}

	D3D12DepthStencilView::~D3D12DepthStencilView()
	{ }

	void D3D12DepthStencilView::CreateDepthStencilView(const D3D12_DEPTH_STENCIL_VIEW_DESC& Desc)
	{
		Device->GetNativeDevice()->CreateDepthStencilView(Resource, &Desc, HeapSlot.Handle);
	}

	D3D12UnorderedAccessView::D3D12UnorderedAccessView(D3D12Device* InDevice, const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc, ID3D12Resource* InResource)
		:D3D12View(InDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, InResource)
	{
		CreateUnorderedAccessView(Desc);
	}

	D3D12UnorderedAccessView::~D3D12UnorderedAccessView()
	{ }

	void D3D12UnorderedAccessView::CreateUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc)
	{
		Device->GetNativeDevice()->CreateUnorderedAccessView(Resource, nullptr, &Desc, HeapSlot.Handle);
	}
}
