#pragma once

#include <Common.h>

#include "ZDelegate.h"
#include "ZEvent.h"
#include "TSharedPointer.h"
#include "ZSharedPointerTarget.h"
#include "ZThread.h"
#include "ZMutex.h"
#include "THashMap.h"
#include "TMap.h"

#include <Functions.h>

class ZResourceStub;
class ZRuntimeResourceID;
class IResourceInstaller;

enum EResourceReferenceFlags
{
    RESOURCE_REFERENCE_NO_FLAGS = 0,
    RESOURCE_REFERENCE_OFFSET_BITS = 16777215,
    RESOURCE_REFERENCE_OFFSET_BITCOUNT = 24,
    RESOURCE_REFERENCE_FLAG_BITS = -16777216,
    RESOURCE_REFERENCE_TYPE_OF_STREAMING_ENTITY = 268435456,
    RESOURCE_REFERENCE_STATE_STREAMED = 536870912,
    RESOURCE_REFERENCE_MEDIA_STREAMED = 1073741824,
    RESOURCE_REFERENCE_INSTALL_DEPENDENCY = -2147483648,
    RESOURCE_REFERENCE_CYCLIC = 67108864
};

enum EResourceStatus
{
    RESOURCE_STATUS_UNKNOWN = 0,
    RESOURCE_STATUS_LOADING = 1,
    RESOURCE_STATUS_INSTALLING = 2,
    RESOURCE_STATUS_FAILED = 3,
    RESOURCE_STATUS_VALID = 4,
    RESOURCE_STATUS_HEADER_LOADED = 5
};

class ZResourceID
{
  public:
    ZResourceID(const ZString& p_Other) : m_uri(p_Other) {}

    bool IsLibraryResource() const
    {
        return m_uri.IndexOf("resourcelib?") != -1 || m_uri.IndexOf("securelib?") != -1;
    }

    ZString m_uri;
};

class ZRuntimeResourceID
{
  public:
    ZRuntimeResourceID() = default;

    ZRuntimeResourceID(uint64_t p_RuntimeResourceID) : m_IDHigh(p_RuntimeResourceID >> 32), m_IDLow(static_cast<uint32_t>(p_RuntimeResourceID)) {}

    ZRuntimeResourceID(uint32_t p_IDHigh, uint32_t p_IDLow) : m_IDHigh(p_IDHigh), m_IDLow(p_IDLow) {}

    operator uint64_t() const
    {
        return GetID();
    }

    uint64_t GetID() const
    {
        return (static_cast<uint64_t>(m_IDHigh) << 32) | m_IDLow;
    }

    bool operator==(const ZRuntimeResourceID& p_Other) const
    {
        return GetID() == p_Other.GetID();
    }

    bool operator!=(const ZRuntimeResourceID& p_Other) const
    {
        return GetID() != p_Other.GetID();
    }

    static ZRuntimeResourceID Create(uint64_t p_Resource)
    {
        return { p_Resource >> 32, static_cast<uint32_t>(p_Resource) };
    }

    static ZRuntimeResourceID CreateLibraryResourceID(ZRuntimeResourceID p_Library, int32_t p_IndexInLibrary)
    {
        return { p_IndexInLibrary & 0xFFFFFF | 0x80000000, p_Library.m_IDLow };
    }

    bool IsLibraryResource() const
    {
        return m_IDHigh >> 31;
    }

    bool IsLibrary() const
    {
        return (m_IDHigh >> 30) & 1;
    }

    ZRuntimeResourceID GetLibraryRuntimeResourceID() const
    {
        return { 0x40000000, m_IDLow };
    }

    int32_t GetIndexInLibrary() const
    {
        return m_IDHigh & 0xFFFFFF;
    }

    uint32_t m_IDHigh = -1;
    uint32_t m_IDLow = -1;
};

class ZResourcePending;

class IResourceInstaller : public IComponentInterface
{
  public:
    virtual ~IResourceInstaller() = 0;
    virtual void Release(const ZRuntimeResourceID& runtimeResourceID, void* param2, uint32_t param3) = 0;
    virtual void* Allocate(uint32_t param1) const = 0;
    virtual bool Install(ZResourcePending& resourcePending) = 0;
    virtual bool IsStreamInstaller() const = 0;
    virtual bool IsIndirectionInstaller() const = 0;
    virtual bool SupportsAllocate() const = 0;
    virtual void OnOrphanedResource(const ZRuntimeResourceID& runtimeResourceID, void* param2, uint32_t param3) = 0;
};

class ZResourcePtr
{
  public:
    ZResourcePtr() = default;
    ZResourcePtr(const ZResourcePtr& other);
    ~ZResourcePtr();

    void* GetRawPointer() const;

    bool IsReady() const;
    bool Exists() const;
    bool Failed() const;

    void AddStatusChangedListener(const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>& p_Delegate);

    void RemoveStatusChangedListener(const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>& p_Delegate);

    bool operator==(const ZResourcePtr& p_Other) const;

    ZResourceStub* m_pResourceStub = nullptr;
};

template<typename T> class TResourcePtr : public ZResourcePtr
{
  public:
    TResourcePtr() = default;

    TResourcePtr(const ZResourcePtr& p_Other) : ZResourcePtr(p_Other) {}

    T* GetRawPointer() const
    {
        return static_cast<T*>(ZResourcePtr::GetRawPointer());
    }
};

class ZResourceStub
{
  public:
    virtual ~ZResourceStub() = default;

    EResourceStatus GetResourceStatus() const
    {
        return static_cast<EResourceStatus>(m_eResourceStatus);
    }

    void AddRef()
    {
        _InterlockedExchangeAdd(&m_nHeaderRef, 1);
        _InterlockedExchangeAdd(&m_nRef, 1);
    }

    void Release();

    ZResourcePtr GetInstallDependency(uint32_t p_Index) const
    {
        return m_resourceReferences[p_Index].m_pResource;
    }

    IResourceInstaller* GetResourceInstaller() const;

    struct SResourceReference
    {
        EResourceReferenceFlags m_Flags;

        union
        {
            ZResourceStub* m_pStub;
            ZRuntimeResourceID* m_pridMediaResource;
        };

        ZResourcePtr m_pResource;
    };

    ZRuntimeResourceID m_ridResource;
    uint8_t m_eResourceStatus;
    uint8_t m_RequestedLoadStatus;
    int16_t m_nPriority;
    void* m_pResourceData;
    uint32_t m_nResourceTag;
    long m_nRef;
    long m_nHeaderRef;
    TArray<SResourceReference> m_resourceReferences;
    TArray<ZResourceStub*> m_backReferences;
};

inline ZResourcePtr::ZResourcePtr(const ZResourcePtr& p_Other)
{
    m_pResourceStub = p_Other.m_pResourceStub;

    if (m_pResourceStub)
    {
        m_pResourceStub->AddRef();
    }
}

inline ZResourcePtr::~ZResourcePtr()
{
    if (m_pResourceStub)
    {
        m_pResourceStub->Release();

        m_pResourceStub = nullptr;
    }
}

inline void* ZResourcePtr::GetRawPointer() const
{
    if (m_pResourceStub && m_pResourceStub->GetResourceStatus() == RESOURCE_STATUS_VALID)
    {
        return m_pResourceStub->m_pResourceData;
    }

    return nullptr;
}

inline bool ZResourcePtr::IsReady() const
{
    if (!m_pResourceStub)
    {
        return false;
    }

    const EResourceStatus resourceStatus = m_pResourceStub->GetResourceStatus();

    if (resourceStatus == RESOURCE_STATUS_VALID)
    {
        return true;
    }

    return false;
}

inline bool ZResourcePtr::Exists() const
{
    return m_pResourceStub != 0;
}

inline bool ZResourcePtr::Failed() const
{
    return m_pResourceStub && m_pResourceStub->GetResourceStatus() == RESOURCE_STATUS_FAILED;
}

inline void ZResourcePtr::AddStatusChangedListener(const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>& p_Delegate)
{
    Functions::ZResourcePtr_AddStatusChangedListener->Call(this, p_Delegate);
}

inline void ZResourcePtr::RemoveStatusChangedListener(const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>& p_Delegate)
{
    Functions::ZResourcePtr_RemoveStatusChangedListener->Call(this, p_Delegate);
}

inline bool ZResourcePtr::operator==(const ZResourcePtr& p_Other) const
{
    return m_pResourceStub == p_Other.m_pResourceStub;
}

class ZResourceManager : public IComponentInterface
{
  public:
    ZResourcePtr GetResourcePtr(const ZRuntimeResourceID& p_Resource, int p_Priority)
    {
        ZResourcePtr result;
        Functions::ZResourceManager_GetResourcePtr->Call(this, result, p_Resource, p_Priority);

        return result;
    }

    ZResourcePtr LoadResource(const ZRuntimeResourceID& p_Resource)
    {
        ZResourcePtr result;
        Functions::ZResourceManager_LoadResource->Call(this, result, p_Resource);

        return result;
    }

    void Update(bool p_SendStatusChangedNotifications)
    {
        Functions::ZResourceManager_Update->Call(this, p_SendStatusChangedNotifications);
    }

    void ReleaseStub(ZResourceStub* p_ResourceStub)
    {
        Functions::ZResourceManager_ReleaseStub->Call(this, p_ResourceStub);
    }

    IResourceInstaller* GetResourceInstaller(uint32_t p_ResourceType) const
    {
        return Functions::ZResourceManager_GetResourceInstaller->Call(this, p_ResourceType);
    }
};

inline void ZResourceStub::Release()
{
    long referenceCount = _InterlockedDecrement(&m_nRef);

    _InterlockedExchangeAdd(&m_nHeaderRef, -1);

    if (referenceCount == 0)
    {
        Globals::ResourceManager->ReleaseStub(this);
    }
}

inline IResourceInstaller* ZResourceStub::GetResourceInstaller() const
{
    return Globals::ResourceManager->GetResourceInstaller(m_nResourceTag);
}

struct SHeaderLibraryChunk
{
    void SerializeToMemory(ZBinarySerializer& p_BinarySerializer, const uint32_t p_Offset)
    {
        uint32_t libraryIDOffset = p_Offset + offsetof(SHeaderLibraryChunk, sLibraryID);
        uint32_t offsetOffset = p_Offset + offsetof(SHeaderLibraryChunk, nOffset);
        uint32_t flagsOffset = p_Offset + offsetof(SHeaderLibraryChunk, nFlags);
        uint32_t localizationCategoryOffset = p_Offset + offsetof(SHeaderLibraryChunk, localizationCategory);
        uint32_t languagesOffset = p_Offset + offsetof(SHeaderLibraryChunk, sLanguages);
        uint32_t statesOffset = p_Offset + offsetof(SHeaderLibraryChunk, states);
        uint32_t resourceHeadersOffset = p_Offset + offsetof(SHeaderLibraryChunk, resourceHeaders);
        uint32_t ridMappingIndicesOffset = p_Offset + offsetof(SHeaderLibraryChunk, ridMappingIndices);
        uint32_t ridMappingIDsOffset = p_Offset + offsetof(SHeaderLibraryChunk, ridMappingIDs);

        p_BinarySerializer.WriteToMemory(&nOffset, sizeof(uint32_t), offsetOffset);
        p_BinarySerializer.WriteToMemory(&nFlags, sizeof(char), flagsOffset);
        p_BinarySerializer.WriteToMemory(&localizationCategory, sizeof(uint32_t), localizationCategoryOffset);

        sLibraryID.SerializeToMemory(p_BinarySerializer, libraryIDOffset);
        sLanguages.SerializeToMemory(p_BinarySerializer, languagesOffset);
        states.SerializeToMemory(p_BinarySerializer, statesOffset);
        resourceHeaders.SerializeToMemory(p_BinarySerializer, resourceHeadersOffset);
        ridMappingIndices.SerializeToMemory(p_BinarySerializer, ridMappingIndicesOffset);
        ridMappingIDs.SerializeToMemory(p_BinarySerializer, ridMappingIDsOffset);
    }

    ZString sLibraryID;
    uint32_t nOffset;
    char nFlags;
    uint32_t localizationCategory;
    TArray<ZString> sLanguages;
    TArray<int32_t> states;
    TArray<TArray<uint8_t>> resourceHeaders;
    TArray<uint32_t> ridMappingIndices;
    TArray<uint64_t> ridMappingIDs;
};

struct SHeaderLibrary
{
    void SerializeToMemory(ZBinarySerializer& p_BinarySerializer)
    {
        uint32_t chunksOffset = offsetof(SHeaderLibrary, chunks);
        uint32_t externalResourceIdsOffset = offsetof(SHeaderLibrary, externalResourceIds);
        uint32_t ridSourceOffset = offsetof(SHeaderLibrary, ridSource);

        p_BinarySerializer.WriteToMemory(&ridSource, sizeof(int64_t), ridSourceOffset);

        chunks.SerializeToMemory(p_BinarySerializer, chunksOffset);
        externalResourceIds.SerializeToMemory(p_BinarySerializer, externalResourceIdsOffset);
    }

    TArray<SHeaderLibraryChunk> chunks;
    TArray<ZString> externalResourceIds;
    int64_t ridSource;
};

struct SResourceHeaderHeader
{
    uint32_t m_type;
    uint32_t m_nReferencesChunkSize;
    uint32_t m_nStatesChunkSize;
    uint32_t m_nDataSize;
    uint32_t m_nSystemMemoryRequirement;
    uint32_t m_nVideoMemoryRequirement;
};

struct SResourceLibraryEntry
{
    ZResourceStub* pStub;
    uint32_t nDataSize;
};

class ZResourceLibraryInfo
{
  public:
    enum
    {
        GLOBAL_LIBRARIES_STATE = -1,
        NON_STREAMED_LIBRARIES_STATE = -2,
        INIT_LIBRARIES_STATE = -3
    };

    enum
    {
        GLOBAL = 1,
        NON_STREAMED = 2,
        MEDIA_STREAMED = 4,
        INIT_LIBRARY = 8,
        DYNAMIC = 16
    };

    const SResourceLibraryEntry& GetEntry(const uint32_t p_Index) const
    {
        return m_Entries[p_Index];
    }

    bool IsValid(uint32_t p_Index) const
    {
        return m_Entries[p_Index].pStub != 0;
    }

    bool InstallResource(uint32_t p_Index, TSharedPointer<ZResourceDataBuffer> p_ResourceData)
    {
        return Functions::ZResourceLibraryInfo_InstallResource->Call(this, p_Index, p_ResourceData);
    }

    TArray<int32_t> m_StreamingStates;
    ZResourceStub* m_pLibraryStub;
    ZResourceID m_ResourceID;
    ZRuntimeResourceID m_MapResourceID;
    ZRuntimeResourceID m_SourceResourceID;
    TArray<SResourceLibraryEntry> m_Entries;
    char m_Flags;
    uint32_t m_nStartFilePosition;
    TArray<uint32_t> m_EntryFilePositions;
    TArray<ZString> m_Languages;
    int32_t m_LocalizationCategory;
};

class ZBufferBlock
{
  public:
    uint8_t* pData;
    uint32_t nPosition;
    uint32_t nReadSize;
    volatile bool m_bLoaded;
};

class ZResourceLibrarySet
{
  public:
    void RemoveReadyCallback(const ZDelegate<void __cdecl(void)>& p_Delegate)
    {
        Functions::ZResourceLibrarySet_RemoveReadyCallback->Call(this, p_Delegate);
    }

    void Release()
    {
        Functions::ZResourceLibrarySet_Release->Call(this);
    }

  private:
    ZMutex m_Mutex;
    ZMutex m_ExecutingCallbackMutex;
    TArray<ZResourcePtr> m_Libraries;
    TArray<ZDelegate<void __cdecl(void)>> m_Callbacks;
    ZResourcePtr m_pHeaderLibrary;
    bool m_bReleased : 1;
    bool m_bProcessingCallbacks : 1;
};

class ZHeaderLibrary
{
  public:
    ZResourcePtr GetSourceResource() const
    {
        const ZRuntimeResourceID sourceRuntimeResourceID = m_LibraryLoadOrder[0]->m_SourceResourceID;

        return Globals::ResourceManager->GetResourcePtr(sourceRuntimeResourceID, 0);
    }

  private:
    const TArray<ZResourceLibraryInfo*> m_LibraryLoadOrder;
    ZResourceStub* m_pHeaderLibraryStub;
    THashMap<ZRuntimeResourceID, ZEvent<ZRuntimeResourceID const&>, TDefaultHashMapPolicy<ZRuntimeResourceID>> m_ResourceUnloadedEvents;
};

class ZResourceDataBuffer : public ZSharedPointerTarget
{
  public:
    virtual ~ZResourceDataBuffer() = 0;

    void Insert(uint8_t* p_Data, uint32_t p_Size)
    {
        memcpy(&m_pData[m_nSize], p_Data, p_Size);

        m_nSize += p_Size;
    }

    static TSharedPointer<ZResourceDataBuffer> Create(void* p_Data, uint32_t p_Capacity)
    {
        TSharedPointer<ZResourceDataBuffer> result;
        Functions::ZResourceDataBuffer_Create->Call(result, p_Data, p_Capacity);

        return result;
    }

    void Clear()
    {
        m_nSize = 0;
    }

    uint8_t* m_pData;
    uint32_t m_nSize;
    uint32_t m_nCapacity;
    bool m_bOwnsDataPtr;
};

class ZResourceReader : public ZSharedPointerTarget
{
  public:
    virtual ~ZResourceReader() = default;
    virtual uint32_t GetResourceType() const = 0;
    virtual uint32_t GetNumResourceIdentifiers() const = 0;
    virtual ZRuntimeResourceID GetResourceIdentifier(uint32_t lResourceIdentifierIndex) const = 0;
    virtual EResourceReferenceFlags GetResourceFlags(uint32_t lResourceIdentifierIndex) const = 0;
    virtual void FillResourceIdentifierArray(TArray<ZRuntimeResourceID>& resourceIDs) const = 0;
    virtual ZResourcePtr GetInstallTimeDependency(uint32_t lResourceIdentifierIndex) const = 0;

    const void* GetResourceData(uint32_t p_Offset) const
    {
        const uint8_t* data = m_pResourceData.GetTarget()->m_pData;

        return &data[p_Offset];
    }

    ZResourceStub* m_pStub;
    TSharedPointer<ZResourceDataBuffer> m_pResourceData;
    uint32_t m_nResourceDataSize;
};

class ZResourcePending
{
  public:
    ZResourcePtr m_pResource;
    TSharedPointer<ZResourceReader> m_pResourceReader;
    int32_t m_nPriority;
    int32_t m_ResourceHandle;
    int32_t m_nDataStartPosition;
};

class ZResourceLibraryLoader
{
  public:
    void AllocateEntry(IResourceInstaller* p_Installer, uint32_t p_Size, ZRuntimeResourceID p_Resource)
    {
        Functions::ZResourceLibraryLoader_AllocateEntry->Call(this, p_Installer, p_Size, p_Resource);
    }

    bool m_bFinished;
    bool m_bFailed;
    ZResourcePending m_LibraryResource;
    ZResourceLibraryInfo* m_pLibraryEntries;
    ZMutex m_BlocksMutex;
    uint32_t m_nMaxResourceSize;
    uint32_t m_nEntryBytesLeft;
    uint32_t m_nEntryIndex;
    TSharedPointer<ZResourceDataBuffer> m_pFixedBuffer;
    TSharedPointer<ZResourceDataBuffer> m_pCurrentData;
    uint32_t m_nReadPosition;
    uint32_t m_nParsePosition;
    uint32_t m_nLibraryFileSize;
    uint32_t m_nAllocationCount;
};

class LocalResourceIDsResolver
{
  public:
    void RecordMapping(const ZRuntimeResourceID& p_RuntimeResourceID, const ZResourceID& p_ResourceID)
    {
        Functions::LocalResourceIDsResolver_RecordMapping->Call(this, p_RuntimeResourceID, p_ResourceID);
    }

    ZMutex mutex;
    TMap<ZRuntimeResourceID, ZResourceID> runtimeResourceIDsToResourceIDs;
};
