#pragma once

#include <unordered_map>
#include <filesystem>

#include <Glacier/ZResource.h>

#include "Hooks.h"

class ZHeaderLibraryInstaller;
class ZResourcePending;
class ZResourceLibraryInfo;
template<typename T> class TSharedPointer;
class ZResourceDataBuffer;
class ZResourceLibraryLoader;
class ZBufferBlock;
struct SResourceLibraryEntry;
class IResourceInstaller;

class ResourcePatcher
{
  public:
    struct PatchedResource
    {
        unsigned long long runtimeResourceID;
        unsigned int resourceDataOffset;
        unsigned int resourceDataSize;
        std::string patchFileName;
    };

    ResourcePatcher();
    void LoadPatchedResources();
    const PatchedResource* GetPatchedResource(const ZRuntimeResourceID& runtimeResourceID) const;
    void GetPatchedResource(const ZRuntimeResourceID& runtimeResourceID, void*& resourceData, unsigned int& resourceDataSize);
    void SetCurrentHeaderLibraryRuntimeResourceID(const ZRuntimeResourceID& currentHeaderLibraryRuntimeResourceID);
    const bool IsResourcePatched(const ZRuntimeResourceID& runtimeResourceID) const;

  private:
    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        ResourcePatcher, bool, ZHeaderLibraryInstaller_Install, ZHeaderLibraryInstaller* p_HeaderLibraryInstaller, ZResourcePending& p_ResourcePending
    );

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        ResourcePatcher, bool, ZResourceLibraryLoader_ProcessBlock, ZResourceLibraryLoader* p_ResourceLibraryLoader, ZBufferBlock* p_BufferBlock
    );

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(
        ResourcePatcher, void, ZResourceLibraryLoader_AllocateEntry, ZResourceLibraryLoader* p_ResourceLibraryLoader,
        IResourceInstaller* p_ResourceInstaller, uint32_t p_Size, ZRuntimeResourceID p_ResourceID
    );

    DECLARE_THISCALL_DETOUR_WITH_CONTEXT(ResourcePatcher, void, ZResourceLibraryLoader_StartLoading, ZResourceLibraryLoader* p_ResourceLibraryLoader);

    std::unordered_map<unsigned long long, std::vector<PatchedResource>> patchedResources;
    ZRuntimeResourceID currentHeaderLibraryRuntimeResourceID;
};
