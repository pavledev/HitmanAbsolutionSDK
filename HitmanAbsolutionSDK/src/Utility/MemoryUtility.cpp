#include <Glacier/Resource/ZDynamicResourceLibrary.h>

#include <Utility/MemoryUtility.h>

HitmanAbsolutionSDK_API void* AllocateMemory(size_t size)
{
    return new char[size];
}

HitmanAbsolutionSDK_API void FreeMemory(void* ptr)
{
    delete[] ptr;
}

template <typename T>
void FreeObject(T* ptr)
{
    delete ptr;
}

template HitmanAbsolutionSDK_API void FreeObject<ZDynamicResourceLibrary>(ZDynamicResourceLibrary* ptr);
