#include "Glacier/Resource/ZResourceHeaderReader.h"
#include "IO/BinaryReader.h"

ZResourceHeaderReader::ZResourceHeaderReader(const SResourceHeaderHeader& headerHeader, unsigned char* pReferencesChunk)
{
    m_HeaderHeader = &headerHeader;
    m_pReferencesChunk = pReferencesChunk;
}

unsigned int ZResourceHeaderReader::GetNumResourceIdentifiers() const
{
    if (m_pReferencesChunk)
    {
        return *reinterpret_cast<const unsigned int*>(m_pReferencesChunk) & 0x7FFFFFFF;
    }

    return 0;
}

ZRuntimeResourceID ZResourceHeaderReader::GetResourceIdentifier(unsigned int lResourceIdentifierIndex) const
{
    if (lResourceIdentifierIndex == UINT32_MAX)
    {
        return -1;
    }

    ZRuntimeResourceID result;
    BinaryReader binaryReader = BinaryReader(m_pReferencesChunk, m_HeaderHeader->m_nReferencesChunkSize);
    int numberOfReferences = binaryReader.Read<int>();

    if (numberOfReferences < 0)
    {
        binaryReader.Seek(sizeof(long long) * lResourceIdentifierIndex, SeekOrigin::Current);

        unsigned long long hash = binaryReader.Read<unsigned long long>();

        result = ZRuntimeResourceID(hash);
    }
    else
    {
        const unsigned int firstFlagOffset = binaryReader.Read<unsigned int>();
        const unsigned int firstResourceIDOffset = binaryReader.Read<unsigned int>();
        std::string resourceID;

        if (numberOfReferences == 1)
        {
            const unsigned int flag = binaryReader.Read<unsigned int>();

            resourceID = binaryReader.ReadString();
        }
        else
        {
            binaryReader.Seek(firstFlagOffset + lResourceIdentifierIndex * sizeof(int), SeekOrigin::Begin);

            unsigned int resourceIDOffset = binaryReader.Read<unsigned int>() & 0x3FFFFFFF;

            binaryReader.Seek(firstResourceIDOffset + resourceIDOffset, SeekOrigin::Begin);

            resourceID = binaryReader.ReadString();
        }

        result = ZRuntimeResourceID::QueryRuntimeResourceID(resourceID.c_str());
    }

    return result;
}

EResourceReferenceFlags ZResourceHeaderReader::GetResourceFlags(unsigned int lResourceIdentifierIndex) const
{
    BinaryReader binaryReader = BinaryReader(m_pReferencesChunk, m_HeaderHeader->m_nReferencesChunkSize);
    int numberOfReferences = binaryReader.Read<int>();
    EResourceReferenceFlags result;

    if (numberOfReferences < 0)
    {
        unsigned int numberOfReferences2 = GetNumResourceIdentifiers();

        binaryReader.Seek(numberOfReferences2 * sizeof(long long) + lResourceIdentifierIndex, SeekOrigin::Current);

        result = static_cast<EResourceReferenceFlags>(binaryReader.Read<unsigned char>() << 24);
    }
    else
    {
        const unsigned int firstFlagOffset = binaryReader.Read<unsigned int>();

        binaryReader.Seek(firstFlagOffset + lResourceIdentifierIndex * sizeof(int), SeekOrigin::Begin);

        result = static_cast<EResourceReferenceFlags>(binaryReader.Read<unsigned int>() & 0xFF000000);
    }

    return result;
}
