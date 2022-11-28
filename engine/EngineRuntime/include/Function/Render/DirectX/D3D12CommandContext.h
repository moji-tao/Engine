#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12DescriptorCache.h"

namespace Engine
{
	class D3D12Device;

	class D3D12CommandContext
	{
	public:
		D3D12CommandContext(D3D12Device* InDevice);

		~D3D12CommandContext();

	public:
		void CreateCommandContext();

		void DestroyCommandContext();

		ID3D12CommandQueue* GetCommandQueue();

		ID3D12GraphicsCommandList* GetCommandList();

		D3D12DescriptorCache* GetDescriptorCache();

		void ResetCommandAllocator();

		void ResetCommandList();

		void ExecuteCommandLists();

		void FlushCommandQueue();

		void EndFrame();

	public:
		ID3D12GraphicsCommandList* CreateImGuiCommand();

	private:
		D3D12Device* Device = nullptr;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue = nullptr;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandListAlloc = nullptr;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> ImGuiCommandListAlloc = nullptr;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList = nullptr;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> ImGuiCommandList = nullptr;

		std::unique_ptr<D3D12DescriptorCache> DescriptorCache = nullptr;

	private:
		Microsoft::WRL::ComPtr<ID3D12Fence> Fence = nullptr;

		uint64_t CurrentFenceValue = 0;
	};
}
