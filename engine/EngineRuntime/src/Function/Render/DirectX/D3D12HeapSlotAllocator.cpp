#include "EngineRuntime/include/Function/Render/DirectX/D3D12HeapSlotAllocator.h"

#include "EngineRuntime/include/Function/Render/DirectX/DirectXUtil.h"

namespace Engine
{
	D3D12HeapSlotAllocator::D3D12HeapSlotAllocator(ID3D12Device* InDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t NumDescriptorPerHeap)
		:D3DDevice(InDevice), HeapDesc(CreateHeapDesc(Type, NumDescriptorPerHeap)), DescriptorSize(D3DDevice->GetDescriptorHandleIncrementSize(HeapDesc.Type))
	{ }

	D3D12HeapSlotAllocator::~D3D12HeapSlotAllocator()
	{ }

	D3D12HeapSlotAllocator::HeapSlot D3D12HeapSlotAllocator::AllocateHeapSlot()
	{
		int EntryIndex = -1;
		for (int i = 0; i < HeapMap.size(); ++i)
		{
			if (HeapMap[i].FreeList.size() > 0)
			{
				EntryIndex = i;
				break;
			}
		}

		if(EntryIndex == -1)
		{
			AllocateHeap();

			EntryIndex = int(HeapMap.size() - 1);
		}

		HeapEntry& Entry = HeapMap[EntryIndex];
		assert(Entry.FreeList.size() > 0);

		FreeRange& Range = Entry.FreeList.front();
		HeapSlot Slot = { (uint32_t)EntryIndex, Range.Start };
		
		Range.Start += DescriptorSize;
		if(Range.Start == Range.End)
		{
			Entry.FreeList.pop_front();
		}

		return Slot;
	}

	void D3D12HeapSlotAllocator::FreeHeapSlot(const HeapSlot& Slot)
	{
		assert(Slot.HeapIndex < HeapMap.size());
		HeapEntry& Entry = HeapMap[Slot.HeapIndex];

		FreeRange NewRange =
		{
			Slot.Handle.ptr,
			Slot.Handle.ptr + DescriptorSize
		};

		bool bFound = false;
		for (auto Node = Entry.FreeList.begin(); Node != Entry.FreeList.end() && !bFound; Node++)
		{
			FreeRange& Range = *Node;
			assert(Range.Start < Range.End);

			if (Range.Start == NewRange.End) 
			{
				Range.Start = NewRange.Start;
				bFound = true;
			}
			else if (Range.End == NewRange.Start) 
			{
				Range.End = NewRange.End;
				bFound = true;
			}
			else
			{
				assert(Range.End < NewRange.Start || Range.Start > NewRange.Start);
				if (Range.Start > NewRange.Start) 
				{
					Entry.FreeList.insert(Node, NewRange);
					bFound = true;
				}
			}
		}

		if (!bFound)
		{
			Entry.FreeList.push_back(NewRange);
		}

	}

	D3D12_DESCRIPTOR_HEAP_DESC D3D12HeapSlotAllocator::CreateHeapDesc(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t NumDescriptorsPerHeap)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = { };

		desc.Type = Type;
		desc.NumDescriptors = NumDescriptorsPerHeap;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		return desc;
	}

	void D3D12HeapSlotAllocator::AllocateHeap()
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap;
		ThrowIfFailed(D3DDevice->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&Heap)), "创建描述符堆失败");
		SetDebugName(Heap.Get(), L"D3D12HeapSlotAllocator Descriptor Heap");

		DescriptorHandle HeapBase = Heap->GetCPUDescriptorHandleForHeapStart();
		assert(HeapBase.ptr != 0);

		HeapEntry Entry;
		Entry.Heap = Heap;
		Entry.FreeList.push_back({ HeapBase.ptr, HeapBase.ptr + (SIZE_T)HeapDesc.NumDescriptors * DescriptorSize });

		HeapMap.push_back(Entry);
	}
}
