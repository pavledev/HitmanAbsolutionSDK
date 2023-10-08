#include <Utility/MemoryUtility.h>

HitmanAbsolutionSDK_API void* AllocateMemory(size_t size)
{
    return new char[size];
}

HitmanAbsolutionSDK_API void FreeMemory(void* ptr)
{
    delete[] ptr;
}
