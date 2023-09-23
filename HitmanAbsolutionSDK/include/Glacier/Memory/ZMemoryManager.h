#pragma once

#include "IPageAllocator.h"
#include "Utility/Alignment.h"

class ZMemoryManager
{
public:
	IPageAllocator* GetPageAllocator() const;
	IAllocator* GetNormalAllocator() const;

private:
	IPageAllocator* m_pPageAllocator; //0x0
	IAllocator* m_pNormalAllocator; //0x4
	PAD(0x108);
};

static_assert(sizeof(ZMemoryManager) == 0x110);
static_assert(alignof(ZMemoryManager) == 0x4);
