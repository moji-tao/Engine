#pragma once
#include <vector>

namespace Engine
{
	class XMemoryPool
	{
	private:
		using usize = uint32_t;
		union obj
		{
			obj* free_list_link;
			uint32_t size;
		};
	public:
#define __ALIGH 8
		XMemoryPool();
		~XMemoryPool();
		XMemoryPool(const XMemoryPool&) = delete;
		XMemoryPool& operator=(const XMemoryPool&) = delete;

		void* Allocate(usize uSize);
		void Deallocate(void* buffer);

	private:
		void* priAllocate(usize uSize);
		void priDeallocate(void* buffer);
		obj* priSwapMemory(usize uSize);

	private:
		usize m_uPoolMaxBlockSize;
		usize m_uPoolBloackLength;
		static size_t ROUND_UP(size_t bytes);

	private:
		obj* m_pFreeList = nullptr;

		struct XPoolNode
		{
			char* m_pStart;
			char* m_pCurrent;
			char* m_pEnd;
		};
		std::vector<XPoolNode> m_xPool;
	};

	void* GetBuffer(size_t uSize);

	void PushBuffer(void* buffer);
}


