#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

namespace Engine
{
	class D3D12BuddyAllocator;

	class D3D12Resource
	{
	public:
		D3D12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> InD3DResource, D3D12_RESOURCE_STATES InitState = D3D12_RESOURCE_STATE_COMMON);

		void Map();

	public:
		Microsoft::WRL::ComPtr<ID3D12Resource> D3DResource = nullptr;

		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = 0;

		D3D12_RESOURCE_STATES CurrentState;

		void* MappedBaseAddress = nullptr;
	};

	struct D3D12BuddyBlockData
	{
		uint32_t Offset = 0;
		uint32_t Order = 0;
		uint32_t ActualUsedSize = 0;

		D3D12Resource* PlacedResource = nullptr;
	};

	class D3D12ResourceLocation
	{
	public:
		enum class ResourceLocationType
		{
			Undefined,
			StandAlone,
			SubAllocation,
		};

	public:
		D3D12ResourceLocation();

		~D3D12ResourceLocation();

		void ReleaseResource();

		void SetType(ResourceLocationType Type) { ResourceLocationType = Type; }

	public:
		ResourceLocationType ResourceLocationType = ResourceLocationType::Undefined;

		D3D12BuddyAllocator* Allocator = nullptr;

		D3D12BuddyBlockData BlockData;
		
		D3D12Resource* UnderlyingResource = nullptr;

		union
		{
			uint64_t OffsetFromBaseOfResource;
			uint64_t OffsetFromBaseOfHeap;
		};

		D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = 0;

		void* MappedAddress = nullptr;
	};

}
