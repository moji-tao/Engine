#include "EngineRuntime/include/Function/Render/DirectX/D3D12CommandContext.h"

#include "EngineRuntime/include/Function/Render/DirectX/D3D12Device.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"

namespace Engine
{
	D3D12CommandContext::D3D12CommandContext(D3D12Device* InDevice)
		:Device(InDevice)
	{
		CreateCommandContext();

		DescriptorCache = std::make_unique<D3D12DescriptorCache>(Device);
	}

	D3D12CommandContext::~D3D12CommandContext()
	{
		DestroyCommandContext();
	}

	void D3D12CommandContext::CreateCommandContext()
	{
		ThrowIfFailed(Device->GetNativeDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(Fence.ReleaseAndGetAddressOf())), "创建Fence失败");

		D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
		QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ThrowIfFailed(Device->GetNativeDevice()->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(CommandQueue.ReleaseAndGetAddressOf())), "创建命令队列失败");
		CommandQueue->SetName(L"场景CommandQueue");

		ThrowIfFailed(Device->GetNativeDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandListAlloc.ReleaseAndGetAddressOf())), "创建命令分配器失败");

		ThrowIfFailed(Device->GetNativeDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandListAlloc.Get(), nullptr, IID_PPV_ARGS(CommandList.ReleaseAndGetAddressOf())), "创建命令列表失败");
		CommandList->SetName(L"场景CommandList");

		ThrowIfFailed(CommandList->Close(), "命令列表关闭失败");
	}

	void D3D12CommandContext::DestroyCommandContext()
	{ }

	ID3D12CommandQueue* D3D12CommandContext::GetCommandQueue()
	{
		return CommandQueue.Get();
	}

	ID3D12GraphicsCommandList* D3D12CommandContext::GetCommandList()
	{
		return CommandList.Get();
	}

	D3D12DescriptorCache* D3D12CommandContext::GetDescriptorCache()
	{
		return DescriptorCache.get();
	}

	void D3D12CommandContext::ResetCommandAllocator()
	{
		ThrowIfFailed(CommandListAlloc->Reset(), "命令分配器重置失败");

		if (ImGuiCommandListAlloc != nullptr)
		{
			ThrowIfFailed(ImGuiCommandListAlloc->Reset(), "ImGui命令分配器重置失败");
		}
	}

	void D3D12CommandContext::ResetCommandList()
	{
		ThrowIfFailed(CommandList->Reset(CommandListAlloc.Get(), nullptr), "命令列表重置失败");

		if (ImGuiCommandList != nullptr)
		{
			ThrowIfFailed(ImGuiCommandList->Reset(ImGuiCommandListAlloc.Get(), nullptr), "ImGui命令列表重置失败");
		}
	}

	void D3D12CommandContext::ExecuteCommandLists()
	{
		if (ImGuiCommandList == nullptr)
		{
			ThrowIfFailed(CommandList->Close(), "命令列表关闭失败");
			ID3D12CommandList* cmdLists[] = { CommandList.Get() };
			CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		}
		else
		{
			ThrowIfFailed(CommandList->Close(), "命令列表关闭失败");
			ThrowIfFailed(ImGuiCommandList->Close(), "ImGui命令列表关闭失败");
			ID3D12CommandList* cmdLists[] = { CommandList.Get(), ImGuiCommandList.Get() };
			CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
		}
	}

	void D3D12CommandContext::FlushCommandQueue()
	{
		++CurrentFenceValue;

		ThrowIfFailed(CommandQueue->Signal(Fence.Get(), CurrentFenceValue), "");

		if (Fence->GetCompletedValue() < CurrentFenceValue)
		{
			HANDLE eventHandle = CreateEvent(nullptr, false, false, nullptr);

			ThrowIfFailed(Fence->SetEventOnCompletion(CurrentFenceValue, eventHandle), "");

			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	void D3D12CommandContext::EndFrame()
	{
		DescriptorCache->Reset();
	}

	ID3D12GraphicsCommandList* D3D12CommandContext::CreateImGuiCommand()
	{
		ThrowIfFailed(Device->GetNativeDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(ImGuiCommandListAlloc.ReleaseAndGetAddressOf())), "创建ImGui命令分配器失败");

		ThrowIfFailed(Device->GetNativeDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandListAlloc.Get(), nullptr, IID_PPV_ARGS(ImGuiCommandList.ReleaseAndGetAddressOf())), "创建ImGui命令列表失败");
		ImGuiCommandList->SetName(L"ImGui CommandList");

		ThrowIfFailed(ImGuiCommandList->Close(), "ImGui命令列表关闭失败");

		return ImGuiCommandList.Get();
	}
}
