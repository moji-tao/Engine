#include <dxgi.h>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Device.h"

#include "EngineRuntime/include/Function/Render/DirectX/D3D12RHI.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"

namespace Engine
{
	D3D12Device::D3D12Device(D3D12RHI* InD3D12RHI)
		:D3DRHI(InD3D12RHI)
	{
		Initialze();
	}

	D3D12Device::~D3D12Device()
	{ }

	ID3D12Device* D3D12Device::GetNativeDevice()
	{
		return D3DDevice.Get();
	}

	D3D12CommandContext* D3D12Device::GetCommandContext()
	{
		return CommandContext.get();
	}

	ID3D12CommandQueue* D3D12Device::GetCommandQueue()
	{
		return CommandContext->GetCommandQueue();
	}

	ID3D12GraphicsCommandList* D3D12Device::GetCommandList()
	{
		return CommandContext->GetCommandList();
	}

	D3D12UploadBufferAllocator* D3D12Device::GetUploadBufferAllocator()
	{
		return UploadBufferAllocator.get();
	}

	D3D12DefaultBufferAllocator* D3D12Device::GetDefaultBufferAllocator()
	{
		return DefaultBufferAllocator.get();
	}

	D3D12TextureResourceAllocator* D3D12Device::GetTextureResourceAllocator()
	{
		return TextureResourceAllocator.get();
	}

	D3D12HeapSlotAllocator* D3D12Device::GetHeapSlotAllocator(D3D12_DESCRIPTOR_HEAP_TYPE HeapType)
	{
		switch (HeapType)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
			return RTVHeapSlotAllocator.get();

		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			return SRVHeapSlotAllocator.get();

		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
			return DSVHeapSlotAllocator.get();

		default:
			return nullptr;
		}
	}

	void D3D12Device::Initialze()
	{
		HRESULT HardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(D3DDevice.ReleaseAndGetAddressOf()));

		if(FAILED(HardwareResult))
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter> WarpAdapter;

			ThrowIfFailed(D3DRHI->GetDxgiFactory()->EnumWarpAdapter(IID_PPV_ARGS(WarpAdapter.ReleaseAndGetAddressOf())), "");
			
			ThrowIfFailed(D3D12CreateDevice(WarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(D3DDevice.ReleaseAndGetAddressOf())), "");
		}

		CommandContext = std::make_unique<D3D12CommandContext>(this);

		UploadBufferAllocator = std::make_unique<D3D12UploadBufferAllocator>(D3DDevice.Get());

		DefaultBufferAllocator = std::make_unique<D3D12DefaultBufferAllocator>(D3DDevice.Get());

		TextureResourceAllocator = std::make_unique<D3D12TextureResourceAllocator>(D3DDevice.Get());

		RTVHeapSlotAllocator = std::make_unique<D3D12HeapSlotAllocator>(D3DDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 200);

		DSVHeapSlotAllocator = std::make_unique<D3D12HeapSlotAllocator>(D3DDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 200);

		SRVHeapSlotAllocator = std::make_unique<D3D12HeapSlotAllocator>(D3DDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 200);
	}
}
