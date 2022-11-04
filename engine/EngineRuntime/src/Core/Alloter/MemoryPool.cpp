#include <cstdlib>
#include <cstring>
#include "EngineRuntime/include/Core/Alloter/MemoryPool.h"

namespace Engine
{
	XMemoryPool::XMemoryPool()
	{
		m_uPoolMaxBlockSize = 1024;
		m_uPoolBloackLength = m_uPoolMaxBlockSize / __ALIGH;
		usize uLength = m_uPoolBloackLength * sizeof(obj);
		for (int i = 0; i < m_uPoolBloackLength; ++i)
		{
			uLength += 20 * __ALIGH * (i + 1);
		}

		char* start = (char*)malloc(uLength * 2);
		::memset(start, 0x00, uLength * 2);
		m_xPool.push_back({ start,start,start + uLength * 2 });

		m_pFreeList = (obj*)m_xPool[0].m_pCurrent;
		m_xPool[0].m_pCurrent += m_uPoolBloackLength * sizeof(obj);
		for (int i = 0; i < m_uPoolBloackLength; ++i)
		{
			usize uBlockSize = __ALIGH * (i + 1);
			usize uLength = 20 * uBlockSize;
			m_pFreeList[i].free_list_link = (obj*)m_xPool[0].m_pCurrent;
			m_xPool[0].m_pCurrent += uLength;
			obj* node = m_pFreeList[i].free_list_link;
			for (int j = 1; j < 20; ++j)
			{
				node->free_list_link = (obj*)(((char*)node) + uBlockSize);
				node = node->free_list_link;
			}
		}
	}
	XMemoryPool::~XMemoryPool()
	{
		for (int i = 0; i < m_xPool.size(); ++i)
		{
			::free(m_xPool[i].m_pStart);
		}
	}
	// 8 16 24 32 40 48 56 64 72 80 88 96 104 112 120 128 ... 256 ... 512 ...
	// 128 block
	void* XMemoryPool::Allocate(usize uSize)
	{
		uSize += 4;
		if (uSize > m_uPoolMaxBlockSize)
		{
			return priAllocate(uSize);
		}
		uSize = ROUND_UP(uSize);
		usize uIndex = uSize / __ALIGH - 1;

		if (m_pFreeList[uIndex].free_list_link == nullptr)
		{
			m_pFreeList[uIndex].free_list_link = priSwapMemory(uSize * 2);
			m_pFreeList[uIndex].free_list_link->free_list_link = (obj*)((char*)(m_pFreeList[uIndex].free_list_link) + uSize);
			m_pFreeList[uIndex].free_list_link->free_list_link->free_list_link = nullptr;
		}
		obj* res = m_pFreeList[uIndex].free_list_link;
		m_pFreeList[uIndex].free_list_link = res->free_list_link;
		res->size = uSize;
		void* result = (char*)(res)+4;
		return result;
	}

	void XMemoryPool::Deallocate(void* buffer)
	{
		usize* uSize = (usize*)buffer;
		uSize--;
		if (*uSize > m_uPoolMaxBlockSize)
		{
			priDeallocate(uSize);
			return;
		}
		usize uIndex = *uSize / __ALIGH - 1;
		obj* rtn = (obj*)uSize;
		rtn->free_list_link = m_pFreeList[uIndex].free_list_link;
		m_pFreeList[uIndex].free_list_link = rtn;
	}

	void* XMemoryPool::priAllocate(usize uSize)
	{
		usize* buffer = (usize*)::malloc(uSize);
		*buffer = uSize;
		return buffer + 1;
	}

	void XMemoryPool::priDeallocate(void* buffer)
	{
		::free(buffer);
		return;
	}

	XMemoryPool::obj* XMemoryPool::priSwapMemory(usize uSize)
	{
		if (uSize > m_uPoolMaxBlockSize)
		{
			bool flag = false;
			for (int i = 0; i < m_xPool.size(); ++i)
			{
				if (m_xPool[i].m_pEnd - m_xPool[i].m_pCurrent >= uSize)
				{
					obj* res = (obj*)m_xPool[i].m_pCurrent;
					m_xPool[i].m_pCurrent += uSize;
					return res;
				}
			}
			char* buffer = (char*)::malloc(uSize * 2);
			m_xPool.push_back({ buffer,buffer + uSize,buffer + uSize * 2 });
			return (obj*)buffer;
		}
		usize uIndex = uSize / __ALIGH - 1;
		if (m_pFreeList[uIndex].free_list_link == nullptr)
		{
			m_pFreeList[uIndex].free_list_link = priSwapMemory(uSize * 2);
			m_pFreeList[uIndex].free_list_link->free_list_link = (obj*)((char*)(m_pFreeList[uIndex].free_list_link) + uSize);
			m_pFreeList[uIndex].free_list_link->free_list_link->free_list_link = nullptr;
		}
		obj* res = m_pFreeList[uIndex].free_list_link;
		m_pFreeList[uIndex].free_list_link = res->free_list_link;
		return res;
	}

	size_t XMemoryPool::ROUND_UP(size_t bytes)
	{
		return (((bytes)+__ALIGH - 1) & ~(__ALIGH - 1));
	}

	XMemoryPool g_MemoryPool;

	void* GetBuffer(size_t uSize)
	{
		void* buffer = g_MemoryPool.Allocate(uSize);
		memset(buffer, 0x00, uSize);
		return buffer;
	}

	void PushBuffer(void* buffer)
	{
		g_MemoryPool.Deallocate(buffer);
	}
}
