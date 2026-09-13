#pragma once

#include "ZResource.h"

#include "SDK.h"

class ZResourceHeaderReader
{
  public:
    ZResourceHeaderReader(const SResourceHeaderHeader& p_HeaderHeader, uint8_t* p_ReferencesChunk)
        : m_HeaderHeader(&p_HeaderHeader), m_pReferencesChunk(p_ReferencesChunk)
    {}

    uint32_t GetNumResourceIdentifiers() const
    {
        if (m_pReferencesChunk)
        {
            return *reinterpret_cast<const uint32_t*>(m_pReferencesChunk) & 0x7FFFFFFF;
        }

        return 0;
    }

    ZRuntimeResourceID GetResourceIdentifier(uint32_t p_ResourceIdentifierIndex) const
    {
        if (p_ResourceIdentifierIndex == UINT32_MAX)
        {
            return -1;
        }

        ZRuntimeResourceID result;
        BinaryReader binaryReader = BinaryReader(m_pReferencesChunk, m_HeaderHeader->m_nReferencesChunkSize);
        int32_t numberOfReferences = binaryReader.Read<int32_t>();

        if (numberOfReferences < 0)
        {
            binaryReader.Seek(sizeof(uint64_t) * p_ResourceIdentifierIndex, SeekOrigin::Current);

            uint64_t hash = binaryReader.Read<uint64_t>();

            result = ZRuntimeResourceID(hash);
        }
        else
        {
            const uint32_t firstFlagOffset = binaryReader.Read<uint32_t>();
            const uint32_t firstResourceIDOffset = binaryReader.Read<uint32_t>();
            std::string resourceID;

            if (numberOfReferences == 1)
            {
                const uint32_t flag = binaryReader.Read<uint32_t>();

                resourceID = binaryReader.ReadString();
            }
            else
            {
                binaryReader.Seek(firstFlagOffset + p_ResourceIdentifierIndex * sizeof(uint32_t), SeekOrigin::Begin);

                uint32_t resourceIDOffset = binaryReader.Read<uint32_t>() & 0x3FFFFFFF;

                binaryReader.Seek(firstResourceIDOffset + resourceIDOffset, SeekOrigin::Begin);

                resourceID = binaryReader.ReadString();
            }

            result = SDK::GetInstance().GetRuntimeResourceID(resourceID.c_str());
        }

        return result;
    }

    EResourceReferenceFlags GetResourceFlags(uint32_t p_ResourceIdentifierIndex) const
    {
        BinaryReader binaryReader = BinaryReader(m_pReferencesChunk, m_HeaderHeader->m_nReferencesChunkSize);
        int32_t numberOfReferences = binaryReader.Read<int32_t>();
        EResourceReferenceFlags result;

        if (numberOfReferences < 0)
        {
            uint32_t numberOfReferences2 = GetNumResourceIdentifiers();

            binaryReader.Seek(numberOfReferences2 * sizeof(uint64_t) + p_ResourceIdentifierIndex, SeekOrigin::Current);

            result = static_cast<EResourceReferenceFlags>(binaryReader.Read<uint8_t>() << 24);
        }
        else
        {
            const uint32_t firstFlagOffset = binaryReader.Read<uint32_t>();

            binaryReader.Seek(firstFlagOffset + p_ResourceIdentifierIndex * sizeof(uint32_t), SeekOrigin::Begin);

            result = static_cast<EResourceReferenceFlags>(binaryReader.Read<uint32_t>() & 0xFF000000);
        }

        return result;
    }

    const SResourceHeaderHeader* m_HeaderHeader;
    uint8_t* m_pReferencesChunk;
};
