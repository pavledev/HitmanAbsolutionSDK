#pragma once

#include <cstdint>

#include "IComponentInterface.h"

#include "Common.h"

struct SPageAllocatorStats;
struct StatCollector;
class IAllocator;

class IAllocator : public IComponentInterface
{
  public:
    virtual ~IAllocator() = 0;
    virtual uint32_t GetDefaultAlignment() = 0;
    virtual bool SupportsAlignedAllocations() = 0;
    virtual void* Allocate(uint32_t param1, uint32_t param2) = 0;
    virtual void* AllocateAligned(uint32_t param1, uint32_t param2, uint32_t param3) = 0;
    virtual void Free(void* param1) = 0;
    virtual uint32_t GetAllocationSize(void* param1) = 0;
    virtual void* ReAllocate(void* param1, uint32_t param2, uint32_t param3) = 0;
    virtual void* ReAllocateAligned(void* param1, uint32_t param2, uint32_t param3, uint32_t param4) = 0;
    virtual void CheckHeap() = 0;
};

class IPageAllocator
{
  public:
    virtual ~IPageAllocator() = 0;
    virtual uint32_t GetPageSize() = 0;
    virtual uint32_t GetReserveGranularity() = 0;
    virtual int GetStats(SPageAllocatorStats& pageAllocatorStats, int32_t param2) = 0;
    virtual void ResetLowestFreeStat() = 0;
    virtual void* Reserve(uint32_t param1) = 0;
    virtual bool Release(void* param1, uint32_t param2) = 0;
    virtual bool Release(void* param1) = 0;
    virtual bool Commit(void* param1, uint32_t param2, StatCollector* statCollector) = 0;
    virtual bool Decommit(void* param1, uint32_t param2) = 0;
    virtual void* GetPhysicalPoolBase() = 0;
    virtual uint32_t GetPhysicalPoolMaxSize() = 0;
    virtual uint32_t ResizePhysicalPool(uint32_t param1) = 0;
    virtual void* GetSwappedPoolBase() = 0;
    virtual uint32_t ResizeSwappedPool(uint32_t param1) = 0;
    virtual void RegisterAllocatorAddress(void* param1, uint32_t param2, IAllocator* allocator) = 0;
    virtual void UnregisterAllocatorAddress(void* param1) = 0;
    virtual IAllocator* GetAllocator(void* param1) = 0;
    virtual int RegisterCollector(StatCollector* statCollector, int32_t param2) = 0;
};

class ZMemoryManager
{
  public:
    IPageAllocator* m_pPageAllocator; // 0x0
    IAllocator* m_pNormalAllocator;   // 0x4
    PAD(0x108);                       // 0x8
};

static_assert(sizeof(ZMemoryManager) == 0x110);
static_assert(alignof(ZMemoryManager) == 0x4);
