#include "Glacier/Memory/ZMemoryManager.h"

IPageAllocator* ZMemoryManager::GetPageAllocator() const
{
	return m_pPageAllocator;
}

IAllocator* ZMemoryManager::GetNormalAllocator() const
{
	return m_pNormalAllocator;
}
