#pragma once

#include "../IComponentInterface.h"

class IAllocator : public IComponentInterface
{
public:
	virtual ~IAllocator() = 0;
	virtual unsigned int GetDefaultAlignment() = 0;
	virtual bool SupportsAlignedAllocations() = 0;
	virtual void* Allocate(unsigned int param1, unsigned int param2) = 0;
	virtual void* AllocateAligned(unsigned int param1, unsigned int param2, unsigned int param3) = 0;
	virtual void Free(void* param1) = 0;
	virtual unsigned int GetAllocationSize(void* param1) = 0;
	virtual void* ReAllocate(void* param1, unsigned int param2, unsigned int param3) = 0;
	virtual void* ReAllocateAligned(void* param1, unsigned int param2, unsigned int param3, unsigned int param4) = 0;
	virtual void CheckHeap() = 0;
};
