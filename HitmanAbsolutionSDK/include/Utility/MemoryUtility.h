#pragma once

#include <Common.h>

class ZDynamicResourceLibrary;

HitmanAbsolutionSDK_API void* AllocateMemory(size_t size);
HitmanAbsolutionSDK_API void FreeMemory(void* ptr);

template <typename T>
void FreeObject(T* ptr);

extern template HitmanAbsolutionSDK_API void FreeObject<ZDynamicResourceLibrary>(ZDynamicResourceLibrary* ptr);
