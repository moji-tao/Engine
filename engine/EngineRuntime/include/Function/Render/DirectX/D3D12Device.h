#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12CommandContext.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12MemoryAllocator.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12HeapSlotAllocator.h"

namespace Engine
{
	class D3D12RHI;

	class D3D12Device
	{
	public:
		D3D12Device(D3D12RHI* InD3D12RHI);

		~D3D12Device();

	public:
		ID3D12Device* GetNativeDevice();

		D3D12CommandContext* GetCommandContext();

		ID3D12CommandQueue* GetCommandQueue();

		ID3D12GraphicsCommandList* GetCommandList();

		D3D12UploadBufferAllocator* GetUploadBufferAllocator();

		D3D12DefaultBufferAllocator* GetDefaultBufferAllocator();

		D3D12TextureResourceAllocator* GetTextureResourceAllocator();

		D3D12HeapSlotAllocator* GetHeapSlotAllocator(D3D12_DESCRIPTOR_HEAP_TYPE HeapType);

	private:
		void Initialze();

	private:
		D3D12RHI* D3DRHI = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Device> D3DDevice = nullptr;

		std::unique_ptr<D3D12CommandContext> CommandContext = nullptr;

	private:
		std::unique_ptr<D3D12UploadBufferAllocator> UploadBufferAllocator = nullptr;

		std::unique_ptr<D3D12DefaultBufferAllocator> DefaultBufferAllocator = nullptr;

		std::unique_ptr<D3D12TextureResourceAllocator> TextureResourceAllocator = nullptr;

		std::unique_ptr<D3D12HeapSlotAllocator> RTVHeapSlotAllocator = nullptr;

		std::unique_ptr<D3D12HeapSlotAllocator> DSVHeapSlotAllocator = nullptr;

		std::unique_ptr<D3D12HeapSlotAllocator> SRVHeapSlotAllocator = nullptr;
	};
}
