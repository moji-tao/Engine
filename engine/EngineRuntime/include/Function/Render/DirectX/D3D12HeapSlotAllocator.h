#pragma once
#include <cstdint>
#include <d3d12.h>
#include <list>
#include <vector>
#include <wrl.h>

namespace Engine
{
	/**
	 * 描述符分配器
	 * 构造时指定要构造的描述符堆类型，内部会维护一个描述符堆，调用AllocateHeapSlot() 会返回一个该类型的描述符句柄
	 */

	class D3D12HeapSlotAllocator
	{
	public:
		typedef D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHandle;
		typedef decltype(DescriptorHandle::ptr) DescriptorHandleRaw;

		struct HeapSlot
		{
			uint32_t HeapIndex;
			D3D12_CPU_DESCRIPTOR_HANDLE Handle;
		};

	private:
		struct FreeRange
		{
			DescriptorHandleRaw Start;
			DescriptorHandleRaw End;
		};

		struct HeapEntry
		{
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Heap = nullptr;
			std::list<D3D12HeapSlotAllocator::FreeRange> FreeList;

			HeapEntry() { }
		};

	public:
		D3D12HeapSlotAllocator(ID3D12Device* InDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t NumDescriptorPerHeap);

		~D3D12HeapSlotAllocator();

		HeapSlot AllocateHeapSlot();

		void FreeHeapSlot(const HeapSlot& Slot);

	private:
		D3D12_DESCRIPTOR_HEAP_DESC CreateHeapDesc(D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t NumDescriptorsPerHeap);

		void AllocateHeap();

	private:
		ID3D12Device* D3DDevice;

		const D3D12_DESCRIPTOR_HEAP_DESC HeapDesc;

		const uint32_t DescriptorSize;

		std::vector<HeapEntry> HeapMap;
	};
}
