#pragma once
#include "EngineRuntime/include/Function/Render/DirectX/d3dx12.h"
#include <wrl.h>

namespace Engine
{
	class D3D12Device;

	class D3D12DescriptorCache
	{
	public:
		D3D12DescriptorCache(D3D12Device* InDevice);

		~D3D12DescriptorCache();

	public:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCacheCbvSrvUavDescriptorHeap();

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCacheRtvDescriptorHeap();

		CD3DX12_GPU_DESCRIPTOR_HANDLE AppendCbvSrvUavDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& SrcDescriptors);

		void AppendRtvDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& RtvDescriptors, CD3DX12_GPU_DESCRIPTOR_HANDLE& OutGpuHandle, CD3DX12_CPU_DESCRIPTOR_HANDLE& OutCpuHandle);

		void Reset();

	private:
		void CreateCacheCbvSrvUavDescriptorHeap();

		void CreateCacheRtvDescriptorHeap();

		void ResetCacheCbvSrvUavDescriptorHeap();

		void ResetCacheRtvDescriptorHeap();

	private:
		D3D12Device* Device = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CacheCbvSrvUavDescriptorHeap = nullptr;

		UINT CbvSrvUavDescriptorSize;

		static const int MaxCbvSrvUavDescripotrCount = 2048;

		uint32_t CbvSrvUavDescriptorOffset = 0;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CacheRtvDescriptorHeap = nullptr;

		UINT RtvDescriptorSize;

		static const int MaxRtvDescriptorCount = 1024;

		uint32_t RtvDescriptorOffset = 0;
	};
}
