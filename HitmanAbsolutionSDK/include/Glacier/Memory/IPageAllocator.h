#pragma once

struct SPageAllocatorStats;
struct StatCollector;
class IAllocator;

class IPageAllocator
{
public:
	virtual ~IPageAllocator() = 0;
	virtual unsigned int GetPageSize() = 0;
	virtual unsigned int GetReserveGranularity() = 0;
	virtual int GetStats(SPageAllocatorStats& pageAllocatorStats, int param2) = 0;
	virtual void ResetLowestFreeStat() = 0;
	virtual void* Reserve(unsigned int param1) = 0;
	virtual bool Release(void* param1, unsigned int param2) = 0;
	virtual bool Release(void* param1) = 0;
	virtual bool Commit(void* param1, unsigned int param2, StatCollector* statCollector) = 0;
	virtual bool Decommit(void* param1, unsigned int param2) = 0;
	virtual void* GetPhysicalPoolBase() = 0;
	virtual unsigned int GetPhysicalPoolMaxSize() = 0;
	virtual unsigned int ResizePhysicalPool(unsigned int param1) = 0;
	virtual void* GetSwappedPoolBase() = 0;
	virtual unsigned int ResizeSwappedPool(unsigned int param1) = 0;
	virtual void RegisterAllocatorAddress(void* param1, unsigned int param2, IAllocator* allocator) = 0;
	virtual void UnregisterAllocatorAddress(void* param1) = 0;
	virtual IAllocator* GetAllocator(void* param1) = 0;
	virtual int RegisterCollector(StatCollector* statCollector, int param2) = 0;
};
