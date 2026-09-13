#include <map>
#include <format>
#include <unordered_set>

#include <Glacier/TSharedPointer.h>
#include <Glacier/ZResource.h>

#include <ResourcePatcher.h>
#include <IO/BinaryReader.h>
#include <SDK.h>
#include <Hooks.h>
#include <Globals.h>

ResourcePatcher::ResourcePatcher()
{
    Hooks::ZHeaderLibraryInstaller_Install->AddDetour(this, &ResourcePatcher::ZHeaderLibraryInstaller_Install);
    Hooks::ZResourceLibraryLoader_ProcessBlock->AddDetour(this, &ResourcePatcher::ZResourceLibraryLoader_ProcessBlock);
    Hooks::ZResourceLibraryLoader_AllocateEntry->AddDetour(this, &ResourcePatcher::ZResourceLibraryLoader_AllocateEntry);
    Hooks::ZResourceLibraryLoader_StartLoading->AddDetour(this, &ResourcePatcher::ZResourceLibraryLoader_StartLoading);
}

void ResourcePatcher::LoadPatchedResources()
{
    if (patchedResources.contains(currentHeaderLibraryRuntimeResourceID.GetID()))
    {
        return;
    }

    const std::string currentHeaderLibraryResourceID = SDK::GetInstance().GetResourceID(currentHeaderLibraryRuntimeResourceID);
    const size_t index2 = currentHeaderLibraryResourceID.find_last_of("/");
    const std::string headerLibraryFileName =
        currentHeaderLibraryResourceID.substr(index2 + 1, currentHeaderLibraryResourceID.find(".", index2) - index2 - 1);
    const std::string patchesFolderPath = std::format("ModManager\\patches\\{}", headerLibraryFileName);

    if (!std::filesystem::is_directory(patchesFolderPath))
    {
        return;
    }

    std::map<uint32_t, std::filesystem::path> patchFiles;

    for (const auto& entry : std::filesystem::directory_iterator(patchesFolderPath))
    {
        const std::string fileName = entry.path().filename().string();
        const uint32_t patchNumber = std::strtoul(fileName.substr(5, fileName.find(".") - 5).c_str(), nullptr, 10);

        patchFiles.insert(std::make_pair(patchNumber, entry.path()));
    }

    std::unordered_set<uint64_t> addedResources;

    for (auto it = patchFiles.begin(); it != patchFiles.end(); ++it)
    {
        BinaryReader binaryReader = BinaryReader(it->second.string());
        const uint32_t patchedResourceCount = binaryReader.Read<uint32_t>();

        auto iterator = patchedResources.insert(std::make_pair(currentHeaderLibraryRuntimeResourceID.GetID(), std::vector<PatchedResource>())).first;

        for (uint32_t i = 0; i < patchedResourceCount; ++i)
        {
            PatchedResource patchedResource;

            patchedResource.runtimeResourceID = binaryReader.Read<uint64_t>();

            if (addedResources.contains(patchedResource.runtimeResourceID))
            {
                binaryReader.Seek(8, SeekOrigin::Current);

                continue;
            }

            patchedResource.resourceDataOffset = binaryReader.Read<uint32_t>();
            patchedResource.resourceDataSize = binaryReader.Read<uint32_t>();
            patchedResource.patchFileName = it->second.filename().string();

            iterator->second.push_back(patchedResource);
            addedResources.insert(patchedResource.runtimeResourceID);
        }
    }
}

const ResourcePatcher::PatchedResource* ResourcePatcher::GetPatchedResource(const ZRuntimeResourceID& runtimeResourceID) const
{
    auto patchedResourcesIterator = patchedResources.find(currentHeaderLibraryRuntimeResourceID.GetID());

    if (patchedResourcesIterator != patchedResources.end())
    {
        const std::vector<PatchedResource>& patchedResources2 = patchedResourcesIterator->second;

        for (size_t i = 0; i < patchedResources2.size(); ++i)
        {
            if (patchedResources2[i].runtimeResourceID == runtimeResourceID.GetID())
            {
                return &patchedResources2[i];
            }
        }
    }

    return nullptr;
}

void ResourcePatcher::GetPatchedResource(const ZRuntimeResourceID& runtimeResourceID, void*& resourceData, uint32_t& resourceDataSize)
{
    const std::vector<PatchedResource>& patchedResources2 = patchedResources[currentHeaderLibraryRuntimeResourceID.GetID()];

    for (size_t i = 0; i < patchedResources2.size(); ++i)
    {
        if (patchedResources2[i].runtimeResourceID == runtimeResourceID.GetID())
        {
            const std::string currentHeaderLibraryResourceID = SDK::GetInstance().GetResourceID(currentHeaderLibraryRuntimeResourceID);
            const size_t index2 = currentHeaderLibraryResourceID.find_last_of("/");
            const std::string headerLibraryFileName =
                currentHeaderLibraryResourceID.substr(index2 + 1, currentHeaderLibraryResourceID.find(".", index2) - index2 - 1);
            const std::string patchFilePath = std::format("ModManager\\patches\\{}\\{}", headerLibraryFileName, patchedResources2[i].patchFileName);
            BinaryReader binaryReader = BinaryReader(patchFilePath);

            binaryReader.Seek(patchedResources2[i].resourceDataOffset);

            resourceData = binaryReader.Read<void>(patchedResources2[i].resourceDataSize);
            resourceDataSize = patchedResources2[i].resourceDataSize;

            break;
        }
    }
}

void ResourcePatcher::SetCurrentHeaderLibraryRuntimeResourceID(const ZRuntimeResourceID& currentHeaderLibraryRuntimeResourceID)
{
    this->currentHeaderLibraryRuntimeResourceID = currentHeaderLibraryRuntimeResourceID;
}

const bool ResourcePatcher::IsResourcePatched(const ZRuntimeResourceID& runtimeResourceID) const
{
    auto patchedResourcesIterator = patchedResources.find(currentHeaderLibraryRuntimeResourceID.GetID());

    if (patchedResourcesIterator != patchedResources.end())
    {
        const std::vector<PatchedResource>& patchedResources2 = patchedResourcesIterator->second;

        for (size_t i = 0; i < patchedResources2.size(); ++i)
        {
            if (patchedResources2[i].runtimeResourceID == runtimeResourceID.GetID())
            {
                return true;
            }
        }
    }

    return false;
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    ResourcePatcher, bool, ZHeaderLibraryInstaller_Install, ZHeaderLibraryInstaller* p_HeaderLibraryInstaller, ZResourcePending& p_ResourcePending
)
{
    SDK::GetInstance().GetResourcePatcher()->SetCurrentHeaderLibraryRuntimeResourceID(p_ResourcePending.m_pResource.m_pResourceStub->m_ridResource);

    return { HookAction::Continue() };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    ResourcePatcher, bool, ZResourceLibraryLoader_ProcessBlock, ZResourceLibraryLoader* p_ResourceLibraryLoader, ZBufferBlock* p_BufferBlock
)
{
    uint32_t nReadSize = p_BufferBlock->nReadSize;
    uint8_t* pData = p_BufferBlock->pData;

    if (nReadSize < p_ResourceLibraryLoader->m_nEntryBytesLeft)
    {
        if (nReadSize)
        {
            const uint32_t entryIndex = p_ResourceLibraryLoader->m_nEntryIndex;
            const ZResourceLibraryInfo* libraryEntries = p_ResourceLibraryLoader->m_pLibraryEntries;
            const SResourceLibraryEntry& entry = libraryEntries->GetEntry(entryIndex);

            if (!IsResourcePatched(entry.pStub->m_ridResource))
            {
                p_ResourceLibraryLoader->m_pCurrentData->Insert(pData, nReadSize);
            }

            p_ResourceLibraryLoader->m_nEntryBytesLeft -= nReadSize;
        }

        return { HookAction::Return(), true };
    }

    while (true)
    {
        const uint32_t entryBytesLeft = p_ResourceLibraryLoader->m_nEntryBytesLeft;
        uint32_t entryIndex = p_ResourceLibraryLoader->m_nEntryIndex;
        ZResourceLibraryInfo* libraryEntries = p_ResourceLibraryLoader->m_pLibraryEntries;
        const SResourceLibraryEntry& entry = libraryEntries->GetEntry(entryIndex);

        if (IsResourcePatched(entry.pStub->m_ridResource))
        {
            void* resourceData = nullptr;
            uint32_t resourceDataSize = 0;

            GetPatchedResource(entry.pStub->m_ridResource, resourceData, resourceDataSize);

            p_ResourceLibraryLoader->m_pCurrentData->Insert(static_cast<uint8_t*>(resourceData), resourceDataSize);

            operator delete(resourceData);
        }
        else
        {
            p_ResourceLibraryLoader->m_pCurrentData->Insert(pData, entryBytesLeft);
        }

        pData += entryBytesLeft;
        nReadSize -= entryBytesLeft;

        if (libraryEntries->IsValid(entryIndex))
        {
            libraryEntries->InstallResource(entryIndex, p_ResourceLibraryLoader->m_pCurrentData);

            if (p_ResourceLibraryLoader->m_pCurrentData.GetTarget())
            {
                p_ResourceLibraryLoader->m_pCurrentData->RemoveReference();
            }

            p_ResourceLibraryLoader->m_pCurrentData.m_pObject = nullptr;
        }

        if (entry.pStub->GetResourceStatus() != RESOURCE_STATUS_VALID)
        {
            if (p_ResourceLibraryLoader->m_pCurrentData.GetTarget())
            {
                p_ResourceLibraryLoader->m_pCurrentData->RemoveReference();
            }

            p_ResourceLibraryLoader->m_pCurrentData.m_pObject = nullptr;
        }

        uint32_t nextEntryIndex = ++entryIndex;

        p_ResourceLibraryLoader->m_nEntryIndex = nextEntryIndex;

        if (nextEntryIndex >= libraryEntries->m_Entries.Size())
        {
            return { HookAction::Return(), false };
        }

        const SResourceLibraryEntry& nextEntry = libraryEntries->GetEntry(nextEntryIndex);
        IResourceInstaller* resourceInstaller = nextEntry.pStub->GetResourceInstaller();
        const ZRuntimeResourceID& runtimeResourceID = nextEntry.pStub->m_ridResource;

        p_ResourceLibraryLoader->AllocateEntry(resourceInstaller, nextEntry.nDataSize, runtimeResourceID);

        if (nReadSize < p_ResourceLibraryLoader->m_nEntryBytesLeft)
        {
            if (nReadSize)
            {
                if (!IsResourcePatched(runtimeResourceID))
                {
                    p_ResourceLibraryLoader->m_pCurrentData->Insert(pData, nReadSize);
                }

                p_ResourceLibraryLoader->m_nEntryBytesLeft -= nReadSize;
            }

            return { HookAction::Return(), true };
        }
    }
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(
    ResourcePatcher, void, ZResourceLibraryLoader_AllocateEntry, ZResourceLibraryLoader* p_ResourceLibraryLoader,
    IResourceInstaller* p_ResourceInstaller, uint32_t p_Size, ZRuntimeResourceID p_ResourceID
)
{
    if (p_ResourceInstaller->SupportsAllocate())
    {
        uint32_t dataSize = 0;
        const PatchedResource* patchedResource = GetPatchedResource(p_ResourceID);

        if (patchedResource)
        {
            dataSize = patchedResource->resourceDataSize;
        }
        else
        {
            dataSize = p_Size;
        }

        void* data = p_ResourceInstaller->Allocate(dataSize);
        TSharedPointer<ZResourceDataBuffer> resourceDataBuffer = ZResourceDataBuffer::Create(data, dataSize);

        if (resourceDataBuffer.GetTarget() != p_ResourceLibraryLoader->m_pCurrentData.GetTarget())
        {
            if (p_ResourceLibraryLoader->m_pCurrentData.GetTarget())
            {
                p_ResourceLibraryLoader->m_pCurrentData->RemoveReference();
            }

            p_ResourceLibraryLoader->m_pCurrentData.m_pObject = resourceDataBuffer.GetTarget();

            if (p_ResourceLibraryLoader->m_pCurrentData.GetTarget())
            {
                p_ResourceLibraryLoader->m_pCurrentData->AddReference();
            }
        }
    }
    else
    {
        if (p_ResourceLibraryLoader->m_pCurrentData.GetTarget() != p_ResourceLibraryLoader->m_pFixedBuffer.GetTarget())
        {
            if (p_ResourceLibraryLoader->m_pCurrentData.GetTarget())
            {
                p_ResourceLibraryLoader->m_pCurrentData->RemoveReference();
            }

            p_ResourceLibraryLoader->m_pCurrentData.m_pObject = p_ResourceLibraryLoader->m_pFixedBuffer.GetTarget();

            if (p_ResourceLibraryLoader->m_pCurrentData.GetTarget())
            {
                p_ResourceLibraryLoader->m_pCurrentData->AddReference();
            }
        }

        p_ResourceLibraryLoader->m_pCurrentData->Clear();
    }

    // Data size of original resource should passed here to avoid problem with reading of resource library
    p_ResourceLibraryLoader->m_nEntryBytesLeft = p_Size;

    return { HookAction::Return() };
}

DEFINE_THISCALL_DETOUR_WITH_CONTEXT(ResourcePatcher, void, ZResourceLibraryLoader_StartLoading, ZResourceLibraryLoader* p_ResourceLibraryLoader)
{
    LoadPatchedResources();

    uint32_t entryIndex = 0;
    uint32_t maxDataSize = 0;
    auto patchedResourcesIterator = patchedResources.find(currentHeaderLibraryRuntimeResourceID.GetID());
    bool reallocateBuffer = false;

    if (patchedResourcesIterator != patchedResources.end())
    {
        uint32_t maxDataSize2 = 0;

        if (p_ResourceLibraryLoader->m_pLibraryEntries->m_Entries.Size() > 0)
        {
            do
            {
                const SResourceLibraryEntry& entry = p_ResourceLibraryLoader->m_pLibraryEntries->GetEntry(entryIndex);

                if (entry.nDataSize > maxDataSize2)
                {
                    IResourceInstaller* resourceInstaller = entry.pStub->GetResourceInstaller();

                    if (!resourceInstaller->SupportsAllocate())
                    {
                        maxDataSize2 = entry.nDataSize;
                    }
                }

                ++entryIndex;
            }
            while (entryIndex < p_ResourceLibraryLoader->m_pLibraryEntries->m_Entries.Size());
        }

        maxDataSize = maxDataSize2;

        const ZRuntimeResourceID& libraryRuntimeResourceID = p_ResourceLibraryLoader->m_pLibraryEntries->m_pLibraryStub->m_ridResource;
        const std::vector<PatchedResource>& patchedResources = patchedResourcesIterator->second;

        for (size_t i = 0; i < patchedResources.size(); ++i)
        {
            if (ZRuntimeResourceID(patchedResources[i].runtimeResourceID).m_IDLow != libraryRuntimeResourceID.m_IDLow)
            {
                continue;
            }

            if (patchedResources[i].resourceDataSize > maxDataSize2)
            {
                maxDataSize2 = patchedResources[i].resourceDataSize;
            }
        }

        if (maxDataSize != maxDataSize2)
        {
            reallocateBuffer = true;
        }

        maxDataSize = maxDataSize2;
    }

    if (reallocateBuffer)
    {
        IAllocator* normalAllocator = (*Globals::MemoryManager)->m_pNormalAllocator;

        // normalAllocator->Free(resourceLibraryLoader->GetFixedBuffer()->GetData());

        void* data = normalAllocator->Allocate(maxDataSize, 0);
        TSharedPointer<ZResourceDataBuffer> resourceDataBuffer = ZResourceDataBuffer::Create(data, maxDataSize);

        if (p_ResourceLibraryLoader->m_pFixedBuffer.GetTarget() != resourceDataBuffer.GetTarget())
        {
            resourceDataBuffer->Insert(
                p_ResourceLibraryLoader->m_pFixedBuffer.GetTarget()->m_pData, p_ResourceLibraryLoader->m_pFixedBuffer.GetTarget()->m_nSize
            );

            if (p_ResourceLibraryLoader->m_pFixedBuffer.GetTarget())
            {
                p_ResourceLibraryLoader->m_pFixedBuffer->RemoveReference();
            }

            p_ResourceLibraryLoader->m_pFixedBuffer.m_pObject = resourceDataBuffer.GetTarget();

            if (p_ResourceLibraryLoader->m_pFixedBuffer.GetTarget())
            {
                p_ResourceLibraryLoader->m_pFixedBuffer->AddReference();
            }
        }
    }

    return { HookAction::Continue() };
}
