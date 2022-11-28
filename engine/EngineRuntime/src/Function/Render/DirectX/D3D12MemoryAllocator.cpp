#include "EngineRuntime/include/Core/Math/Math.h"
#include "EngineRuntime/include/Function/Render/DirectX/D3D12MemoryAllocator.h"
#include "EngineRuntime/include/Function/Render/DirectX/d3dx12.h"
#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"

namespace Engine
{
	D3D12BuddyAllocator::D3D12BuddyAllocator(ID3D12Device* InDevice, const AllocatorInitData& InInitData)
		: D3DDevice(InDevice), InitData(InInitData)
	{
		Initialize();
	}

	D3D12BuddyAllocator::~D3D12BuddyAllocator()
	{
		if (BackingResource)
		{
			delete BackingResource;
		}
		
		if (BackingHeap)
		{
			BackingHeap->Release();
		}
	}

	bool D3D12BuddyAllocator::AllocResource(uint32_t Size, uint32_t Alignment, D3D12ResourceLocation& ResourceLocation)
	{
		uint32_t SizeToAllocate = GetSizeToAllocate(Size, Alignment);

		if (CanAllocate(SizeToAllocate))
		{
			// Allocate block
			const uint32_t UnitSize = SizeToUnitSize(SizeToAllocate);
			const uint32_t Order = UnitSizeToOrder(UnitSize);
			const uint32_t Offset = AllocateBlock(Order); // This is the offset in MinBlockSize units
			const uint32_t AllocSize = UnitSize * MinBlockSize;
			TotalAllocSize += AllocSize;

			//Calculate AlignedOffsetFromResourceBase
			const uint32_t OffsetFromBaseOfResource = GetAllocOffsetInBytes(Offset);
			uint32_t AlignedOffsetFromResourceBase = OffsetFromBaseOfResource;
			if (Alignment != 0 && OffsetFromBaseOfResource % Alignment != 0)
			{
				AlignedOffsetFromResourceBase = AlignArbitrary(OffsetFromBaseOfResource, Alignment);

				uint32_t Padding = AlignedOffsetFromResourceBase - OffsetFromBaseOfResource;
				assert((Padding + Size) <= AllocSize);
			}
			assert((AlignedOffsetFromResourceBase % Alignment) == 0);

			// Save allocation info to ResourceLocation
			ResourceLocation.SetType(D3D12ResourceLocation::ResourceLocationType::SubAllocation);
			ResourceLocation.BlockData.Order = Order;
			ResourceLocation.BlockData.Offset = Offset;
			ResourceLocation.BlockData.ActualUsedSize = Size;
			ResourceLocation.Allocator = this;

			if (InitData.AllocationStrategy == AllocationStrategy::ManualSubAllocation)
			{
				ResourceLocation.UnderlyingResource = BackingResource;
				ResourceLocation.OffsetFromBaseOfResource = AlignedOffsetFromResourceBase;
				ResourceLocation.GPUVirtualAddress = BackingResource->GPUVirtualAddress + AlignedOffsetFromResourceBase;

				if (InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
				{
					ResourceLocation.MappedAddress = ((uint8_t*)BackingResource->MappedBaseAddress + AlignedOffsetFromResourceBase);
				}
			}
			else
			{
				ResourceLocation.OffsetFromBaseOfHeap = AlignedOffsetFromResourceBase;

				// Place resource are initialized by caller
			}

			return true;
		}
		else
		{
			return false;
		}

	}

	void D3D12BuddyAllocator::Deallocate(D3D12ResourceLocation& ResourceLocation)
	{
		DeferredDeletionQueue.push_back(ResourceLocation.BlockData);
	}

	void D3D12BuddyAllocator::CleanUpAllocations()
	{
		for (int32_t i = 0; i < DeferredDeletionQueue.size(); i++)
		{
			const D3D12BuddyBlockData& Block = DeferredDeletionQueue[i];

			DeallocateInternal(Block);
		}

		// clear out all of the released blocks, don't allow the array to shrink
		DeferredDeletionQueue.clear();
	}

	ID3D12Heap* D3D12BuddyAllocator::GetBackingHeap()
	{
		return BackingHeap;
	}

	D3D12BuddyAllocator::AllocationStrategy D3D12BuddyAllocator::GetAllocationStrategy()
	{
		return InitData.AllocationStrategy;
	}

	void D3D12BuddyAllocator::Initialize()
	{
		//Create BackingHeap or BackingResource
		if (InitData.AllocationStrategy == AllocationStrategy::PlacedResource)
		{
			CD3DX12_HEAP_PROPERTIES HeapProperties(InitData.HeapType);
			D3D12_HEAP_DESC Desc = {};
			Desc.SizeInBytes = DEFAULT_POOL_SIZE;
			Desc.Properties = HeapProperties;
			Desc.Alignment = 0;
			Desc.Flags = InitData.HeapFlags;

			// Create BackingHeap, we will create place resources on it.
			ID3D12Heap* Heap = nullptr;
			ThrowIfFailed(D3DDevice->CreateHeap(&Desc, IID_PPV_ARGS(&Heap)), "创建D3DHeap失败");
			Heap->SetName(L"TD3D12BuddyAllocator BackingHeap");

			BackingHeap = Heap;
		}
		else //ManualSubAllocation
		{
			CD3DX12_HEAP_PROPERTIES HeapProperties(InitData.HeapType);
			D3D12_RESOURCE_STATES HeapResourceStates;
			if (InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
			{
				HeapResourceStates = D3D12_RESOURCE_STATE_GENERIC_READ;
			}
			else //D3D12_HEAP_TYPE_DEFAULT
			{
				HeapResourceStates = D3D12_RESOURCE_STATE_COMMON;
			}

			CD3DX12_RESOURCE_DESC BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(DEFAULT_POOL_SIZE, InitData.ResourceFlags);

			// Create committed resource, we will allocate sub regions on it.
			Microsoft::WRL::ComPtr<ID3D12Resource> Resource;
			ThrowIfFailed(D3DDevice->CreateCommittedResource(
				&HeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&BufferDesc,
				HeapResourceStates,
				nullptr,
				IID_PPV_ARGS(&Resource)), "创建D3DResource失败");

			Resource->SetName(L"TD3D12BuddyAllocator BackingResource");

			BackingResource = new D3D12Resource(Resource);

			if (InitData.HeapType == D3D12_HEAP_TYPE_UPLOAD)
			{
				BackingResource->Map();
			}
		}

		// Initialize free blocks, add the free block for MaxOrder
		MaxOrder = UnitSizeToOrder(SizeToUnitSize(DEFAULT_POOL_SIZE));

		for (uint32_t i = 0; i <= MaxOrder; i++)
		{
			FreeBlocks.emplace_back(std::set<uint32_t>());
		}

		FreeBlocks[MaxOrder].insert((uint32_t)0);

	}

	uint32_t D3D12BuddyAllocator::AllocateBlock(uint32_t Order)
	{
		uint32_t Offset;

		if (Order > MaxOrder)
		{
			assert(false);
		}

		if (FreeBlocks[Order].size() == 0)
		{
			// No free nodes in the requested pool.  Try to find a higher-order block and split it.  
			uint32_t Left = AllocateBlock(Order + 1);

			uint32_t UnitSize = OrderToUnitSize(Order);

			uint32_t Right = Left + UnitSize;

			FreeBlocks[Order].insert(Right); // Add the right block to the free pool  

			Offset = Left; // Return the left block  
		}
		else
		{
			auto It = FreeBlocks[Order].cbegin();
			Offset = *It;

			// Remove the block from the free list
			FreeBlocks[Order].erase(*It);
		}

		return Offset;
	}

	uint32_t D3D12BuddyAllocator::GetSizeToAllocate(uint32_t Size, uint32_t Alignment)
	{
		uint32_t SizeToAllocate = Size;

		// If the alignment doesn't match the block size
		if (Alignment != 0 && MinBlockSize % Alignment != 0)
		{
			SizeToAllocate = Size + Alignment;
		}

		return SizeToAllocate;
	}

	bool D3D12BuddyAllocator::CanAllocate(uint32_t SizeToAllocate)
	{
		if (TotalAllocSize == DEFAULT_POOL_SIZE)
		{
			return false;
		}

		uint32_t BlockSize = DEFAULT_POOL_SIZE;

		for (int i = (int)FreeBlocks.size() - 1; i >= 0; i--)
		{
			if (FreeBlocks[i].size() && BlockSize >= SizeToAllocate)
			{
				return true;
			}

			// Halve the block size;
			BlockSize = BlockSize >> 1;

			if (BlockSize < SizeToAllocate) return false;
		}
		return false;
	}

	uint32_t D3D12BuddyAllocator::SizeToUnitSize(uint32_t Size) const
	{
		return (Size + (MinBlockSize - 1)) / MinBlockSize;
	}

	uint32_t D3D12BuddyAllocator::UnitSizeToOrder(uint32_t Size) const
	{
		unsigned long Result;
		_BitScanReverse(&Result, Size + Size - 1);
		return Result;
	}

	uint32_t D3D12BuddyAllocator::OrderToUnitSize(uint32_t Order) const
	{
		return ((uint32_t)1) << Order;
	}

	void D3D12BuddyAllocator::DeallocateInternal(const D3D12BuddyBlockData& Block)
	{
		DeallocateBlock(Block.Offset, Block.Order);

		uint32_t Size = OrderToUnitSize(Block.Order) * MinBlockSize;
		TotalAllocSize -= Size;

		if (InitData.AllocationStrategy == AllocationStrategy::PlacedResource)
		{
			// Release place resource
			assert(Block.PlacedResource != nullptr);

			delete Block.PlacedResource;
		}
	}

	void D3D12BuddyAllocator::DeallocateBlock(uint32_t Offset, uint32_t Order)
	{
		// Get buddy block
		uint32_t Size = OrderToUnitSize(Order);
		uint32_t Buddy = GetBuddyOffset(Offset, Size);

		auto It = FreeBlocks[Order].find(Buddy);
		if (It != FreeBlocks[Order].end()) // If buddy block is free, merge it
		{
			// Deallocate merged blocks
			DeallocateBlock(Math::Min(Offset, Buddy), Order + 1);

			// Remove the buddy from the free list  
			FreeBlocks[Order].erase(*It);
		}
		else
		{
			// Add the block to the free list
			FreeBlocks[Order].insert(Offset);
		}
	}

	uint32_t D3D12BuddyAllocator::GetBuddyOffset(const uint32_t& Offset, const uint32_t& Size)
	{
		return Offset ^ Size;
	}

	uint32_t D3D12BuddyAllocator::GetAllocOffsetInBytes(uint32_t Offset) const
	{
		return Offset * MinBlockSize;
	}

	D3D12MultiBuddyAllocator::D3D12MultiBuddyAllocator(ID3D12Device* InDevice, const D3D12BuddyAllocator::AllocatorInitData& InInitData)
		: mDevice(InDevice), mInitData(InInitData)
	{ }

	D3D12MultiBuddyAllocator::~D3D12MultiBuddyAllocator()
	{ }

	bool D3D12MultiBuddyAllocator::AllocResource(uint32_t Size, uint32_t Alignment, D3D12ResourceLocation& ResourceLocation)
	{
		for (auto& Allocator : mAllocators) // Try to use existing allocators 
		{
			if (Allocator->AllocResource(Size, Alignment, ResourceLocation))
			{
				return true;
			}
		}

		// Create new allocator
		auto Allocator = std::make_shared<D3D12BuddyAllocator>(mDevice, mInitData);
		mAllocators.push_back(Allocator);

		bool Result = Allocator->AllocResource(Size, Alignment, ResourceLocation);
		assert(Result);

		return true;
	}

	void D3D12MultiBuddyAllocator::CleanUpAllocations()
	{
		for (auto& Allocator : mAllocators)
		{
			Allocator->CleanUpAllocations();
		}
	}

	D3D12UploadBufferAllocator::D3D12UploadBufferAllocator(ID3D12Device* InDevice)
	{
		D3D12BuddyAllocator::AllocatorInitData InitData;
		InitData.AllocationStrategy = D3D12BuddyAllocator::AllocationStrategy::ManualSubAllocation;
		InitData.HeapType = D3D12_HEAP_TYPE_UPLOAD;
		InitData.ResourceFlags = D3D12_RESOURCE_FLAG_NONE;

		mAllocator = std::make_unique<D3D12MultiBuddyAllocator>(InDevice, InitData);

		mDevice = InDevice;
	}

	void* D3D12UploadBufferAllocator::AllocUploadResource(uint32_t Size, uint32_t Alignment, D3D12ResourceLocation& ResourceLocation)
	{
		mAllocator->AllocResource(Size, Alignment, ResourceLocation);

		return ResourceLocation.MappedAddress;
	}

	void D3D12UploadBufferAllocator::CleanUpAllocations()
	{
		mAllocator->CleanUpAllocations();
	}

	D3D12DefaultBufferAllocator::D3D12DefaultBufferAllocator(ID3D12Device* InDevice)
	{
		{
			D3D12BuddyAllocator::AllocatorInitData InitData;
			InitData.AllocationStrategy = D3D12BuddyAllocator::AllocationStrategy::ManualSubAllocation;
			InitData.HeapType = D3D12_HEAP_TYPE_DEFAULT;
			InitData.ResourceFlags = D3D12_RESOURCE_FLAG_NONE;

			mAllocator = std::make_unique<D3D12MultiBuddyAllocator>(InDevice, InitData);
		}

		{
			D3D12BuddyAllocator::AllocatorInitData InitData;
			InitData.AllocationStrategy = D3D12BuddyAllocator::AllocationStrategy::ManualSubAllocation;
			InitData.HeapType = D3D12_HEAP_TYPE_DEFAULT;
			InitData.ResourceFlags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

			mUavAllocator = std::make_unique<D3D12MultiBuddyAllocator>(InDevice, InitData);
		}

		mDevice = InDevice;
	}

	void D3D12DefaultBufferAllocator::AllocDefaultResource(const D3D12_RESOURCE_DESC& ResourceDesc, uint32_t Alignment, D3D12ResourceLocation& ResourceLocation)
	{
		if (ResourceDesc.Flags == D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
		{
			mUavAllocator->AllocResource((uint32_t)ResourceDesc.Width, Alignment, ResourceLocation);
		}
		else
		{
			mAllocator->AllocResource((uint32_t)ResourceDesc.Width, Alignment, ResourceLocation);
		}

	}

	void D3D12DefaultBufferAllocator::CleanUpAllocations()
	{
		mAllocator->CleanUpAllocations();

		mUavAllocator->CleanUpAllocations();
	}

	D3D12TextureResourceAllocator::D3D12TextureResourceAllocator(ID3D12Device* InDevice)
	{
		D3D12BuddyAllocator::AllocatorInitData InitData;
		InitData.AllocationStrategy = D3D12BuddyAllocator::AllocationStrategy::PlacedResource;
		InitData.HeapType = D3D12_HEAP_TYPE_DEFAULT;
		InitData.HeapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;

		mAllocator = std::make_unique<D3D12MultiBuddyAllocator>(InDevice, InitData);

		mDevice = InDevice;
	}

	void D3D12TextureResourceAllocator::AllocTextureResource(const D3D12_RESOURCE_STATES& ResourceState, const D3D12_RESOURCE_DESC& ResourceDesc, D3D12ResourceLocation& ResourceLocation)
	{
		const D3D12_RESOURCE_ALLOCATION_INFO Info = mDevice->GetResourceAllocationInfo(0, 1, &ResourceDesc);

		mAllocator->AllocResource((uint32_t)Info.SizeInBytes, DEFAULT_RESOURCE_ALIGNMENT, ResourceLocation);

		// Create placed resource
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> Resource;
			ID3D12Heap* BackingHeap = ResourceLocation.Allocator->GetBackingHeap();
			uint64_t HeapOffset = ResourceLocation.OffsetFromBaseOfHeap;
			mDevice->CreatePlacedResource(BackingHeap, HeapOffset, &ResourceDesc, ResourceState, nullptr, IID_PPV_ARGS(&Resource));

			D3D12Resource* NewResource = new D3D12Resource(Resource);
			ResourceLocation.UnderlyingResource = NewResource;
			ResourceLocation.BlockData.PlacedResource = NewResource;  // Will delete Resource when ResourceLocation was destroyed
		}
	}

	void D3D12TextureResourceAllocator::CleanUpAllocations()
	{
		mAllocator->CleanUpAllocations();
	}
}
