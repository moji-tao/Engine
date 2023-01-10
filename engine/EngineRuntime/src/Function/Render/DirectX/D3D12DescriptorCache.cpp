#include "EngineRuntime/include/Function/Render/DirectX/D3D12DescriptorCache.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Device.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"
#include "EngineRuntime/include/Core/Base/macro.h"

namespace Engine
{
	D3D12DescriptorCache::D3D12DescriptorCache(D3D12Device* InDevice)
		:Device(InDevice)
	{
		CreateCacheCbvSrvUavDescriptorHeap();

		CreateCacheRtvDescriptorHeap();
	}

	D3D12DescriptorCache::~D3D12DescriptorCache()
	{ }

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> D3D12DescriptorCache::GetCacheCbvSrvUavDescriptorHeap()
	{
		return CacheCbvSrvUavDescriptorHeap;
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> D3D12DescriptorCache::GetCacheRtvDescriptorHeap()
	{
		return CacheRtvDescriptorHeap;
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorCache::AppendCbvSrvUavDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& SrcDescriptors)
	{
		uint32_t SlotsNeeded = (uint32_t)SrcDescriptors.size();
		ASSERT(CbvSrvUavDescriptorOffset + SlotsNeeded < MaxCbvSrvUavDescripotrCount);

		auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), CbvSrvUavDescriptorOffset, CbvSrvUavDescriptorSize);
		Device->GetNativeDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotsNeeded, SlotsNeeded, SrcDescriptors.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		
		auto GpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CacheCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), CbvSrvUavDescriptorOffset, CbvSrvUavDescriptorSize);

		CbvSrvUavDescriptorOffset += SlotsNeeded;

		return GpuDescriptorHandle;
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorCache::AppendCbvSrvUavDescriptor(const D3D12_CPU_DESCRIPTOR_HANDLE& SrcDescriptor)
	{
		uint32_t SlotsNeeded = 1;

		auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), CbvSrvUavDescriptorOffset, CbvSrvUavDescriptorSize);
		Device->GetNativeDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotsNeeded, SlotsNeeded, &SrcDescriptor, nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		auto GpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CacheCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), CbvSrvUavDescriptorOffset, CbvSrvUavDescriptorSize);

		CbvSrvUavDescriptorOffset += SlotsNeeded;

		return GpuDescriptorHandle;

	}
	/*
	void D3D12DescriptorCache::AppendRtvDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& RtvDescriptors, CD3DX12_GPU_DESCRIPTOR_HANDLE& OutGpuHandle, CD3DX12_CPU_DESCRIPTOR_HANDLE& OutCpuHandle)
	{
		// Append to heap
		uint32_t SlotsNeeded = (uint32_t)RtvDescriptors.size();
		ASSERT(RtvDescriptorOffset + SlotsNeeded < MaxRtvDescriptorCount);

		auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);
		Device->GetNativeDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotsNeeded, SlotsNeeded, RtvDescriptors.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		OutGpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);

		OutCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);

		// Increase descriptor offset
		RtvDescriptorOffset += SlotsNeeded;
	}
	*/
	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorCache::AppendRtvDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& RtvDescriptors)
	{
		// Append to heap
		uint32_t SlotsNeeded = (uint32_t)RtvDescriptors.size();
		ASSERT(RtvDescriptorOffset + SlotsNeeded < MaxRtvDescriptorCount);

		auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);
		Device->GetNativeDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotsNeeded, SlotsNeeded, RtvDescriptors.data(), nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE OutCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);

		// Increase descriptor offset
		RtvDescriptorOffset += SlotsNeeded;

		return OutCpuHandle;
	}
	/*
	void D3D12DescriptorCache::AppendRtvDescriptor(const D3D12_CPU_DESCRIPTOR_HANDLE& RtvDescriptor, CD3DX12_GPU_DESCRIPTOR_HANDLE& OutGpuHandle, CD3DX12_CPU_DESCRIPTOR_HANDLE& OutCpuHandle)
	{
		// Append to heap
		uint32_t SlotsNeeded = 1;
		ASSERT(RtvDescriptorOffset + SlotsNeeded < MaxRtvDescriptorCount);

		auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);
		Device->GetNativeDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotsNeeded, SlotsNeeded, &RtvDescriptor, nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		OutGpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);

		OutCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);

		// Increase descriptor offset
		RtvDescriptorOffset += SlotsNeeded;

	}
	*/
	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorCache::AppendRtvDescriptor(const D3D12_CPU_DESCRIPTOR_HANDLE& RtvDescriptor)
	{
		// Append to heap
		uint32_t SlotsNeeded = 1;
		ASSERT(RtvDescriptorOffset + SlotsNeeded < MaxRtvDescriptorCount);

		auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);
		Device->GetNativeDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotsNeeded, SlotsNeeded, &RtvDescriptor, nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE OutCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);

		// Increase descriptor offset
		RtvDescriptorOffset += SlotsNeeded;

		return OutCpuHandle;
	}

	void D3D12DescriptorCache::Reset()
	{
		ResetCacheCbvSrvUavDescriptorHeap();

		ResetCacheRtvDescriptorHeap();
	}

	void D3D12DescriptorCache::CreateCacheCbvSrvUavDescriptorHeap()
	{
		// Create the descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc = {};
		SrvHeapDesc.NumDescriptors = MaxCbvSrvUavDescripotrCount;
		SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		ThrowIfFailed(Device->GetNativeDevice()->CreateDescriptorHeap(&SrvHeapDesc, IID_PPV_ARGS(&CacheCbvSrvUavDescriptorHeap)), "");
		SetDebugName(CacheCbvSrvUavDescriptorHeap.Get(), L"TD3D12DescriptorCache CacheCbvSrvUavDescriptorHeap");

		CbvSrvUavDescriptorSize = Device->GetNativeDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void D3D12DescriptorCache::CreateCacheRtvDescriptorHeap()
	{
		// Create the descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC RtvHeapDesc = {};
		RtvHeapDesc.NumDescriptors = MaxRtvDescriptorCount;
		RtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		RtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		ThrowIfFailed(Device->GetNativeDevice()->CreateDescriptorHeap(&RtvHeapDesc, IID_PPV_ARGS(&CacheRtvDescriptorHeap)), "");
		SetDebugName(CacheRtvDescriptorHeap.Get(), L"TD3D12DescriptorCache CacheRtvDescriptorHeap");

		RtvDescriptorSize = Device->GetNativeDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	void D3D12DescriptorCache::ResetCacheCbvSrvUavDescriptorHeap()
	{
		CbvSrvUavDescriptorOffset = 0;
	}

	void D3D12DescriptorCache::ResetCacheRtvDescriptorHeap()
	{
		RtvDescriptorOffset = 0;
	}
}
