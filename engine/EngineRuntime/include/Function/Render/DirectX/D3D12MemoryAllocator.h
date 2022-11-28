#pragma once
#include <memory>
#include <set>
#include <vector>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12Resource.h"

namespace Engine
{
#define DEFAULT_POOL_SIZE (512 * 1024 * 512)

#define DEFAULT_RESOURCE_ALIGNMENT 4
#define UPLOAD_RESOURCE_ALIGNMENT 256

	class D3D12BuddyAllocator
	{
	public:
		enum class AllocationStrategy
		{
			PlacedResource,
			ManualSubAllocation
		};

		struct AllocatorInitData
		{
			AllocationStrategy AllocationStrategy;

			D3D12_HEAP_TYPE HeapType;

			D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE;  // Only for PlacedResource

			D3D12_RESOURCE_FLAGS ResourceFlags = D3D12_RESOURCE_FLAG_NONE;  // Only for ManualSubAllocation
		};

	public:
		D3D12BuddyAllocator(ID3D12Device* InDevice, const AllocatorInitData& InInitData);

		~D3D12BuddyAllocator();

	public:
		bool AllocResource(uint32_t Size, uint32_t Alignment, D3D12ResourceLocation& ResourceLocation);

		void Deallocate(D3D12ResourceLocation& ResourceLocation);

		void CleanUpAllocations();

		ID3D12Heap* GetBackingHeap();

		AllocationStrategy GetAllocationStrategy();

	private:
		void Initialize();

		uint32_t AllocateBlock(uint32_t Order);

		uint32_t GetSizeToAllocate(uint32_t Size, uint32_t Alignment);

		bool CanAllocate(uint32_t SizeToAllocate);

		uint32_t SizeToUnitSize(uint32_t Size) const;

		uint32_t UnitSizeToOrder(uint32_t Size) const;

		uint32_t OrderToUnitSize(uint32_t Order) const;

		void DeallocateInternal(const D3D12BuddyBlockData& Block);

		void DeallocateBlock(uint32_t Offset, uint32_t Order);

		uint32_t GetBuddyOffset(const uint32_t& Offset, const uint32_t& Size);

		uint32_t GetAllocOffsetInBytes(uint32_t Offset) const;

	private:
		AllocatorInitData InitData;

		const uint32_t MinBlockSize = 128;

		uint32_t MaxOrder;

		uint32_t TotalAllocSize = 0;

		std::vector<std::set<uint32_t>> FreeBlocks;

		std::vector<D3D12BuddyBlockData> DeferredDeletionQueue;

		ID3D12Device* D3DDevice;

		D3D12Resource* BackingResource = nullptr;

		ID3D12Heap* BackingHeap = nullptr;
	};

	class D3D12MultiBuddyAllocator
	{
	public:
		D3D12MultiBuddyAllocator(ID3D12Device* InDevice, const D3D12BuddyAllocator::AllocatorInitData& InInitData);

		~D3D12MultiBuddyAllocator();

		bool AllocResource(uint32_t Size, uint32_t Alignment, D3D12ResourceLocation& ResourceLocation);

		void CleanUpAllocations();

	private:
		std::vector<std::shared_ptr<D3D12BuddyAllocator>> mAllocators;

		ID3D12Device* mDevice;

		D3D12BuddyAllocator::AllocatorInitData mInitData;
	};

	class D3D12UploadBufferAllocator
	{
	public:
		D3D12UploadBufferAllocator(ID3D12Device* InDevice);

		void* AllocUploadResource(uint32_t Size, uint32_t Alignment, D3D12ResourceLocation& ResourceLocation);

		void CleanUpAllocations();

	private:
		std::unique_ptr<D3D12MultiBuddyAllocator> mAllocator = nullptr;

		ID3D12Device* mDevice;
	};

	class D3D12DefaultBufferAllocator
	{
	public:
		D3D12DefaultBufferAllocator(ID3D12Device* InDevice);

		void AllocDefaultResource(const D3D12_RESOURCE_DESC& ResourceDesc, uint32_t Alignment, D3D12ResourceLocation& ResourceLocation);

		void CleanUpAllocations();

	private:
		std::unique_ptr<D3D12MultiBuddyAllocator> mAllocator = nullptr;

		std::unique_ptr<D3D12MultiBuddyAllocator> mUavAllocator = nullptr;

		ID3D12Device* mDevice;

	};

	class D3D12TextureResourceAllocator
	{
	public:
		D3D12TextureResourceAllocator(ID3D12Device* InDevice);

		void AllocTextureResource(const D3D12_RESOURCE_STATES& ResourceState, const D3D12_RESOURCE_DESC& ResourceDesc, D3D12ResourceLocation& ResourceLocation);

		void CleanUpAllocations();

	private:
		std::unique_ptr<D3D12MultiBuddyAllocator> mAllocator = nullptr;

		ID3D12Device* mDevice;

	};
}