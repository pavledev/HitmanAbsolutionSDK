#pragma once

#include <filesystem>

#include <Glacier/ZDynamicResourceLibrary.h>

#include "IO/BinaryReader.h"
#include "Resource/DynamicHeaderLibraryGenerator.h"
#include "Utils/StringUtils.h"
#include "SDK.h"

namespace util
{
    inline std::string ConvertResourceIDToFilePath(const std::string& p_ResourceID)
    {
        std::string resourceID = util::ToLowerCase(p_ResourceID);
        const std::string extension = resourceID.substr(resourceID.find_last_of('.') + 1);
        const int index = static_cast<int>(resourceID.find(':'));
        const int index2 = static_cast<int>(resourceID.find('?'));
        const int index3 = static_cast<int>(resourceID.find(']'));
        std::string folderPath = std::format("{}/runtime", std::filesystem::current_path().string());

        if (index2 != -1 && index < index2)
        {
            folderPath += resourceID.substr(index + 1, index2 - index - 1);
        }
        else
        {
            folderPath += resourceID.substr(index + 1, index3 - index - 1);
        }

        resourceID = resourceID.substr(0, resourceID.find_last_of('.') + 1);

        const hash::MD5Hash md5Hash = hash::MD5(resourceID);
        const std::string fileName = hash::MD5ToString(md5Hash);

        return std::format("{}/{}.{}", folderPath, fileName, extension);
    }

    inline void LoadResourceData(
        BinaryReader& p_HeaderLibraryBinaryReader, const uint32_t p_ResourceHeadersStartOffset, const uint32_t p_IndexInLibrary,
        const std::string& p_ResourceLibraryFilePath, void*& p_ResourceData, uint32_t& p_ResourceDataSize
    )
    {
        p_HeaderLibraryBinaryReader.Seek(p_ResourceHeadersStartOffset - 4, SeekOrigin::Begin);

        uint32_t resourceHeaderCount = p_HeaderLibraryBinaryReader.Read<uint32_t>();
        uint32_t resourceLibraryOffset = 0x18;
        uint32_t currentPosition = 0;

        for (uint32_t i = 0; i < resourceHeaderCount; ++i)
        {
            currentPosition = p_HeaderLibraryBinaryReader.GetPosition();
            uint32_t resourceHeaderOffset = currentPosition + p_HeaderLibraryBinaryReader.Read<uint32_t>();
            p_HeaderLibraryBinaryReader.Seek(resourceHeaderOffset, SeekOrigin::Begin);

            uint32_t type = p_HeaderLibraryBinaryReader.Read<uint32_t>();

            p_HeaderLibraryBinaryReader.Skip(8);

            uint32_t dataSize = p_HeaderLibraryBinaryReader.Read<uint32_t>();

            if (type == 'FSBM' || type == 'FSBS')
            {
                dataSize += 24;
            }

            if (i == p_IndexInLibrary)
            {
                BinaryReader resourceLibraryBinaryReader = BinaryReader(p_ResourceLibraryFilePath);

                resourceLibraryBinaryReader.Seek(resourceLibraryOffset);

                p_ResourceDataSize = dataSize;
                p_ResourceData = resourceLibraryBinaryReader.Read<void>(p_ResourceDataSize);

                break;
            }

            resourceLibraryOffset += dataSize;

            p_HeaderLibraryBinaryReader.Seek(currentPosition + 0xC, SeekOrigin::Begin);
        }
    }

    inline void LoadResource(
        const uint64_t p_ResourceRuntimeResourceID, const uint64_t p_HeaderLibraryruntimeResourceID, void*& p_ResourceData,
        uint32_t& p_ResourceDataSize
    )
    {
        const std::string headerLibraryResourceID = SDK::GetInstance().GetResourceID(p_HeaderLibraryruntimeResourceID);
        const std::string headerLibraryFilePath = ConvertResourceIDToFilePath(headerLibraryResourceID);
        BinaryReader headerLibraryBinaryReader = BinaryReader(headerLibraryFilePath);

        headerLibraryBinaryReader.Seek(4, SeekOrigin::Begin);

        const uint32_t referencesChunkSize = headerLibraryBinaryReader.Read<uint32_t>();

        headerLibraryBinaryReader.Seek(referencesChunkSize + 24 + 16, SeekOrigin::Begin);

        uint32_t chunksStartOffset = headerLibraryBinaryReader.GetPosition();
        chunksStartOffset += headerLibraryBinaryReader.Read<uint32_t>();

        headerLibraryBinaryReader.Seek(chunksStartOffset - 4);

        const uint32_t chunkCount = headerLibraryBinaryReader.Read<uint32_t>();

        for (uint32_t i = 0; i < chunkCount; ++i)
        {
            const uint32_t chunkOffset = chunksStartOffset + 0x50 * i; // 0x50 is size of SHeaderLibraryChunk

            headerLibraryBinaryReader.Seek(chunkOffset + 0x44, SeekOrigin::Begin);

            uint32_t runtimeResourceIDsStartOffset = headerLibraryBinaryReader.GetPosition();
            runtimeResourceIDsStartOffset += headerLibraryBinaryReader.Read<uint32_t>();

            headerLibraryBinaryReader.Seek(runtimeResourceIDsStartOffset - 4);

            const uint32_t ridMappingIDsCount = headerLibraryBinaryReader.Read<uint32_t>();
            uint32_t indexInLibrary = -1;

            for (uint32_t j = 0; j < ridMappingIDsCount; ++j)
            {
                headerLibraryBinaryReader.Seek(runtimeResourceIDsStartOffset + j * 0x8, SeekOrigin::Begin);

                const uint64_t runtimeResourceID = headerLibraryBinaryReader.Read<uint64_t>();

                if (runtimeResourceID == p_ResourceRuntimeResourceID)
                {
                    indexInLibrary = j;

                    break;
                }
            }

            headerLibraryBinaryReader.Seek(chunkOffset + 0x4, SeekOrigin::Begin);

            uint32_t headerLibraryChunkResourceIDOffset = headerLibraryBinaryReader.GetPosition();
            headerLibraryChunkResourceIDOffset += headerLibraryBinaryReader.Read<uint32_t>();

            headerLibraryBinaryReader.Seek(headerLibraryChunkResourceIDOffset - 4, SeekOrigin::Begin);

            const uint32_t resourceIDLength = headerLibraryBinaryReader.Read<uint32_t>();
            const std::string headerLibraryChunkResourceID = headerLibraryBinaryReader.ReadString(resourceIDLength - 1);
            const std::string resourceLibraryFilePath = ConvertResourceIDToFilePath(headerLibraryChunkResourceID);

            headerLibraryBinaryReader.Seek(chunkOffset + 0x2C, SeekOrigin::Begin);

            uint32_t resourceHeadersStartOffset = headerLibraryBinaryReader.GetPosition();
            resourceHeadersStartOffset += headerLibraryBinaryReader.Read<uint32_t>();

            LoadResourceData(
                headerLibraryBinaryReader, resourceHeadersStartOffset, indexInLibrary, resourceLibraryFilePath, p_ResourceData, p_ResourceDataSize
            );
        }
    }

    inline std::filesystem::path GetHeaderLibraryPath(const uint64_t p_ResourceRuntimeResourceID)
    {
        for (auto& p : std::filesystem::recursive_directory_iterator("runtime"))
        {
            if (p.path().extension() != ".pc_headerlib")
            {
                continue;
            }

            BinaryReader headerLibraryBinaryReader = BinaryReader(p.path().string());

            headerLibraryBinaryReader.Seek(4, SeekOrigin::Begin);

            const uint32_t referencesChunkSize = headerLibraryBinaryReader.Read<uint32_t>();

            headerLibraryBinaryReader.Seek(referencesChunkSize + 24 + 16, SeekOrigin::Begin);

            uint32_t chunksStartOffset = headerLibraryBinaryReader.GetPosition();
            chunksStartOffset += headerLibraryBinaryReader.Read<uint32_t>();

            headerLibraryBinaryReader.Seek(chunksStartOffset - 4);

            const uint32_t chunkCount = headerLibraryBinaryReader.Read<uint32_t>();

            for (uint32_t i = 0; i < chunkCount; ++i)
            {
                const uint32_t chunkOffset = chunksStartOffset + 0x50 * i; // 0x50 is size of SHeaderLibraryChunk

                headerLibraryBinaryReader.Seek(chunkOffset + 0x44, SeekOrigin::Begin);

                uint32_t runtimeResourceIDsStartOffset = headerLibraryBinaryReader.GetPosition();
                runtimeResourceIDsStartOffset += headerLibraryBinaryReader.Read<uint32_t>();

                headerLibraryBinaryReader.Seek(runtimeResourceIDsStartOffset - 4);

                const uint32_t ridMappingIDsCount = headerLibraryBinaryReader.Read<uint32_t>();
                uint32_t indexInLibrary = -1;

                for (uint32_t j = 0; j < ridMappingIDsCount; ++j)
                {
                    headerLibraryBinaryReader.Seek(runtimeResourceIDsStartOffset + j * 0x8, SeekOrigin::Begin);

                    const uint64_t runtimeResourceID = headerLibraryBinaryReader.Read<uint64_t>();

                    if (runtimeResourceID == p_ResourceRuntimeResourceID)
                    {
                        return p.path();
                    }
                }
            }
        }

        return {};
    }

    inline ZRuntimeResourceID GetTEMPRuntimeResourceID(const ZRuntimeResourceID& p_HeaderLibraryRuntimeResourceID)
    {
        TResourcePtr<ZHeaderLibrary> headerLibraryResourcePtr = Globals::ResourceManager->LoadResource(p_HeaderLibraryRuntimeResourceID);
        const ZResourcePtr tempResourcePtr = headerLibraryResourcePtr.GetRawPointer()->GetSourceResource();
        const ZRuntimeResourceID tempRuntimeResourceID = tempResourcePtr.m_pResourceStub->m_ridResource;

        return tempRuntimeResourceID;
    }

    inline bool InstallDynamicResourceLibrary(
        const ZRuntimeResourceID& p_HeaderLibraryRuntimeResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary,
        ZRuntimeResourceID& p_TempRuntimeResourceID, uint32_t p_EntityCount = 1
    )
    {
        p_TempRuntimeResourceID = GetTEMPRuntimeResourceID(p_HeaderLibraryRuntimeResourceID);

        void* dynamicResourceLibraryMemory = (*Globals::MemoryManager)->m_pNormalAllocator->Allocate(sizeof(ZDynamicResourceLibrary), 0);
        p_DynamicResourceLibrary = new (dynamicResourceLibraryMemory) ZDynamicResourceLibrary(p_HeaderLibraryRuntimeResourceID, false, p_EntityCount);

        if (!p_DynamicResourceLibrary->IsReady())
        {
            p_DynamicResourceLibrary->WaitForResources();
        }

        if (p_DynamicResourceLibrary->IsFailed())
        {
            Logger::Error("Failed install dynamic resource library!");

            p_DynamicResourceLibrary->~ZDynamicResourceLibrary();
            (*Globals::MemoryManager)->m_pNormalAllocator->Free(p_DynamicResourceLibrary);
            p_DynamicResourceLibrary = nullptr;

            return false;
        }

        return true;
    }

    inline bool InstallDynamicResourceLibrary(
        const std::string& p_HeaderLibraryResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary, ZRuntimeResourceID& p_TempRuntimeResourceID,
        const uint32_t p_EntityCount = 1
    )
    {
        const ZRuntimeResourceID headerLibraryRuntimeResourceID = SDK::GetInstance().GetRuntimeResourceID(p_HeaderLibraryResourceID.c_str());

        return InstallDynamicResourceLibrary(headerLibraryRuntimeResourceID, p_DynamicResourceLibrary, p_TempRuntimeResourceID, p_EntityCount);
    }

    inline bool CreateAndInstallDynamicResourceLibrary(
        const std::string& p_ResourceID, ZDynamicResourceLibrary*& p_DynamicResourceLibrary, ZRuntimeResourceID& p_TempRuntimeResourceID,
        const uint32_t p_EntityCount = 1
    )
    {
        ZRuntimeResourceID resourceRuntimeResourceID = SDK::GetInstance().GetRuntimeResourceID(p_ResourceID.c_str());
        std::filesystem::path headerLibraryFilePath = GetHeaderLibraryPath(resourceRuntimeResourceID);
        std::string newHeaderLibraryResourceID;
        DynamicHeaderLibraryGenerator dynamicHeaderLibraryGenerator;

        dynamicHeaderLibraryGenerator.ExtractResourceToHeaderLibrary(p_ResourceID, headerLibraryFilePath.string(), newHeaderLibraryResourceID);

        return InstallDynamicResourceLibrary(newHeaderLibraryResourceID, p_DynamicResourceLibrary, p_TempRuntimeResourceID, p_EntityCount);
    }
};
