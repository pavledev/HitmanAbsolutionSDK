#include <Glacier/ZSerializer.h>
#include <Glacier/ZResource.h>
#include <Glacier/ZObject.h>

ZBinarySerializer::ZBinarySerializer()
{
    this->m_Buffer = nullptr;
    this->m_LayoutPointer = 0;
    this->m_Capacity = 0;
    this->m_MaxAlignment = 0;
}

uint8_t ZBinarySerializer::GetMaxAlignment()
{
    return m_MaxAlignment;
}

uint32_t ZBinarySerializer::GetLayoutPointer()
{
    return m_LayoutPointer;
}

uint32_t
ZBinarySerializer::ReserveLayoutFor(uint32_t p_Count, uint32_t p_TypeSize, uint32_t p_TypeAlignment, uint32_t p_HeaderSize, bool p_UseMaxAlignment)
{
    if (p_TypeAlignment > m_MaxAlignment)
    {
        m_MaxAlignment = p_TypeAlignment;
    }

    uint32_t typeAlignment2;

    if (p_UseMaxAlignment)
    {
        typeAlignment2 = m_MaxAlignment;
    }
    else
    {
        typeAlignment2 = p_TypeAlignment;
    }

    uint32_t result = ~(typeAlignment2 - 1) & (typeAlignment2 + m_LayoutPointer - 1);

    if (result - m_LayoutPointer < p_HeaderSize)
    {
        result = ~(typeAlignment2 - 1) & (m_LayoutPointer + typeAlignment2 + p_HeaderSize - 1);
    }

    m_LayoutPointer = result;

    if (p_Count * p_TypeSize == 0)
    {
        ++m_LayoutPointer;
    }
    else
    {
        m_LayoutPointer += p_Count * p_TypeSize;
    }

    return result;
}

void ZBinarySerializer::SetLayoutPointer(uint32_t p_LayoutPointer)
{
    this->m_LayoutPointer = p_LayoutPointer;
}

void ZBinarySerializer::RecordOffsetForRebasing(uint32_t p_Offset)
{
    m_RebaseLocations.insert(p_Offset);
}

void ZBinarySerializer::RecordOffsetForTypeIDReindexing(uint32_t p_Offset)
{
    m_TypeReindexLocations.push_back(p_Offset);
}

void ZBinarySerializer::RecordRuntimeResourceIDOffset(uint32_t p_Offset)
{
    m_RuntimeResourceIDOffsets.insert(p_Offset);
}

uint32_t ZBinarySerializer::TypeIdToIndex(STypeID* p_TypeID)
{
    uint32_t typeIndex = m_TypeIDs.size();

    for (uint32_t i = 0; i < m_TypeIDs.size(); i++)
    {
        if (m_TypeIDs[i] == p_TypeID)
        {
            typeIndex = i;

            break;
        }
    }

    if (typeIndex == m_TypeIDs.size())
    {
        m_TypeIDs.push_back(p_TypeID);
    }

    return typeIndex;
}

void ZBinarySerializer::WriteToFileStream(const std::string& p_FilePath, std::unordered_map<uint64_t, uint32_t>* p_References)
{
    void* data = nullptr;
    uint32_t dataSize = 0;

    WriteToMemoryStream(&data, dataSize, p_References);

    BinaryWriter binaryWriter = BinaryWriter(p_FilePath);

    binaryWriter.Write(data, dataSize);
}

void ZBinarySerializer::WriteToMemoryStream(void** p_Data, uint32_t& p_DataSize, std::unordered_map<uint64_t, uint32_t>* p_References)
{
    std::vector<ZBinarySerializer::Section> sections = GenerateSections();
    int sizeOfAllSections = 0;

    for (uint32_t i = 0; i < sections.size(); i++)
    {
        sizeOfAllSections += sections[i].m_Size;
    }

    sizeOfAllSections += sections.size() * 4;

    uintptr_t capacity = 16 + m_LayoutPointer + sizeOfAllSections;

    *p_Data = calloc(1, capacity);

    BinaryWriter binaryWriter = BinaryWriter(*p_Data, capacity);

    binaryWriter.Write<uint32_t>(0x324E4942);
    binaryWriter.Write<uint8_t>(BinaryBulkDataHeader::EDataEndianness::FLAG_MASK_LITTLENDIAN);
    binaryWriter.Write<uint8_t>(m_MaxAlignment);
    binaryWriter.Write<uint8_t>(static_cast<uint8_t>(sections.size()));
    binaryWriter.Write<uint8_t>(0);

    const uint8_t dataLength0 = (m_LayoutPointer & 0xFF000000u) >> 24;
    const uint8_t dataLength1 = (m_LayoutPointer & 0xFF0000) >> 16;
    const uint8_t dataLength2 = (m_LayoutPointer & 0xFF00) >> 8;
    const uint8_t dataLength3 = m_LayoutPointer & 0xFFu;

    binaryWriter.Write<uint8_t>(dataLength0);
    binaryWriter.Write<uint8_t>(dataLength1);
    binaryWriter.Write<uint8_t>(dataLength2);
    binaryWriter.Write<uint8_t>(dataLength3);

    binaryWriter.Write<uint32_t>(0);

    uint32_t previousRebaseLocation = 0;
    BinaryWriter rebaseSectionBinaryWriter = BinaryWriter(sections[0].m_Data, sections[0].m_Size);

    rebaseSectionBinaryWriter.Seek(4, SeekOrigin::Begin);

    for (auto it = m_RebaseLocations.begin(); it != m_RebaseLocations.end(); ++it)
    {
        const uint32_t rebaseLocation = *it;
        uint32_t value = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(m_Buffer) + rebaseLocation);

        if (value == 0)
        {
            --value;
        }

        value -= rebaseLocation;

        WriteToMemory(&value, sizeof(uint32_t), rebaseLocation);

        rebaseSectionBinaryWriter.Write<uint32_t>(rebaseLocation - previousRebaseLocation);

        previousRebaseLocation = rebaseLocation;
    }

    if (!m_TypeIDs.empty())
    {
        uint32_t previousTypeReindexLocation = 0;
        BinaryWriter typeReindexingSectionBinaryWriter = BinaryWriter(sections[1].m_Data, sections[1].m_Size);

        typeReindexingSectionBinaryWriter.Seek(4, SeekOrigin::Begin);

        for (uint32_t i = 0; i < m_TypeReindexLocations.size(); ++i)
        {
            const uint32_t typeReindexLocation = m_TypeReindexLocations[i];

            typeReindexingSectionBinaryWriter.Write<uint32_t>(typeReindexLocation - previousTypeReindexLocation);

            previousTypeReindexLocation = typeReindexLocation;
        }
    }

    if (!m_RuntimeResourceIDOffsets.empty())
    {
        BinaryWriter runimeResourceIDReindexingSectionBinaryWriter = BinaryWriter(sections[2].m_Data, sections[2].m_Size);

        runimeResourceIDReindexingSectionBinaryWriter.Seek(4, SeekOrigin::Begin);

        for (auto it = m_RuntimeResourceIDOffsets.begin(); it != m_RuntimeResourceIDOffsets.end(); ++it)
        {
            const uint32_t offset = *it;
            const ZRuntimeResourceID runtimeResourceID = *reinterpret_cast<ZRuntimeResourceID*>(reinterpret_cast<uintptr_t>(m_Buffer) + offset);

            if (runtimeResourceID.GetID() == 0xFFFFFFFFFFFFFFFF)
            {
                *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(m_Buffer) + offset) = 0xFFFFFFFF;
                *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(m_Buffer) + offset + 4) = 0xFFFFFFFF;
            }
            else
            {
                const uint32_t index = (*p_References)[runtimeResourceID.GetID()];

                *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(m_Buffer) + offset) = 0;
                *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(m_Buffer) + offset + 4) = index;
            }

            runimeResourceIDReindexingSectionBinaryWriter.Write<uint32_t>(offset);
        }
    }

    binaryWriter.Write(m_Buffer, m_LayoutPointer);

    for (uint32_t i = 0; i < sections.size(); i++)
    {
        binaryWriter.Write<uint32_t>(sections[i].m_Type);
        binaryWriter.Write<uint32_t>(sections[i].m_Size);
        binaryWriter.Write(sections[i].m_Data, sections[i].m_Size);
    }

    *p_Data = binaryWriter.GetBuffer();
    p_DataSize = binaryWriter.GetPosition();
}

void ZBinarySerializer::WriteToMemory(const void* p_Data, uint32_t p_Size, uint32_t p_Offset)
{
    EnsureCapacity(p_Size + p_Offset);

    void* buffer2 = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_Buffer) + p_Offset);

    memcpy(buffer2, p_Data, p_Size);
}

void ZBinarySerializer::EnsureCapacity(uint32_t p_Size)
{
    if (p_Size > m_Capacity)
    {
        uint32_t newCapacity = m_Capacity * 2;

        while (newCapacity < p_Size)
        {
            newCapacity *= 2;
        }

        m_Buffer = realloc(m_Buffer, newCapacity);

        uint32_t size2 = newCapacity - m_Capacity;
        void* buffer2 = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_Buffer) + m_Capacity);

        memset(buffer2, 0, size2);

        m_Capacity = newCapacity;
    }
}

uint32_t ZBinarySerializer::GetAlignedOffset(uint32_t p_TypeSize, uint32_t p_TypeAlignment)
{
    if (p_TypeAlignment > m_MaxAlignment)
    {
        m_MaxAlignment = p_TypeAlignment;
    }

    uint32_t result = ~(p_TypeAlignment - 1) & (m_LayoutPointer + p_TypeAlignment - 1);

    m_LayoutPointer = result;

    if (!p_TypeSize)
    {
        ++m_LayoutPointer;
    }
    else
    {
        m_LayoutPointer += p_TypeSize;
    }

    return result;
}

uint32_t ZBinarySerializer::GetAlignedLayoutPointer(uint32_t p_LayoutPointer, uint32_t p_Alignment)
{
    return ~(p_Alignment - 1) & (p_LayoutPointer + p_Alignment - 1);
}

std::optional<uint32_t> ZBinarySerializer::GetDataOffset(const ZVariant* p_Variant)
{
    auto it = m_VariantDataOffsets.find(p_Variant->m_TypeID);

    if (it == m_VariantDataOffsets.end())
    {
        return std::nullopt;
    }

    auto& dataPointers = it->second;

    for (auto it2 = dataPointers.begin(); it2 != dataPointers.end(); ++it2)
    {
        if (p_Variant->m_pData == it2->first)
        {
            return std::make_optional(it2->second);
        }
    }

    return std::nullopt;
}

void ZBinarySerializer::SetDataOffset(const ZVariant* p_Variant, uint32_t p_Offset)
{
    auto it = m_VariantDataOffsets.find(p_Variant->m_TypeID);

    if (it != m_VariantDataOffsets.end())
    {
        it->second[p_Variant->m_pData] = p_Offset;

        return;
    }

    std::unordered_map<void*, uint32_t> dataPointers;

    dataPointers[p_Variant->m_pData] = p_Offset;
    m_VariantDataOffsets[p_Variant->m_TypeID] = dataPointers;
}

ZBinarySerializer::Section ZBinarySerializer::GenerateRebaseLocationsSection()
{
    uint32_t capacity = (m_RebaseLocations.size() + 1) * sizeof(uint32_t);
    void* data = calloc(1, capacity);
    BinaryWriter binaryWriter = BinaryWriter(data, capacity);

    binaryWriter.Write<uint32_t>(m_RebaseLocations.size());

    for (auto it = m_RebaseLocations.begin(); it != m_RebaseLocations.end(); ++it)
    {
        binaryWriter.Write<uint32_t>(*it);
    }

    return Section(0x12EBA5ED, binaryWriter.GetPosition(), binaryWriter.GetBuffer());
}

ZBinarySerializer::Section ZBinarySerializer::GenerateTypeReindexingSection()
{
    uint32_t capacity = (m_TypeReindexLocations.size() + m_TypeIDs.size() + 2) * sizeof(uint32_t);
    void* data = calloc(1, capacity);
    BinaryWriter binaryWriter = BinaryWriter(data, capacity);

    binaryWriter.Write<uint32_t>(m_TypeReindexLocations.size());

    for (uint32_t i = 0; i < m_TypeReindexLocations.size(); ++i)
    {
        binaryWriter.Write<uint32_t>(m_TypeReindexLocations[i]);
    }

    binaryWriter.Write<uint32_t>(m_TypeIDs.size());

    for (uint32_t i = 0; i < m_TypeIDs.size(); i++)
    {
        binaryWriter.AlignTo(4);

        binaryWriter.Write<uint32_t>(i);
        binaryWriter.Write<int>(-1);
        binaryWriter.Write<uint32_t>(strlen(m_TypeIDs[i]->pTypeInfo->pszTypeName) + 1);
        binaryWriter.WriteString(m_TypeIDs[i]->pTypeInfo->pszTypeName);
    }

    return Section(0x3989BF9F, binaryWriter.GetPosition(), binaryWriter.GetBuffer());
}

ZBinarySerializer::Section ZBinarySerializer::GenerateRunimeResourceIDReindexingSection()
{
    uint32_t capacity = (m_RuntimeResourceIDOffsets.size() + 1) * sizeof(uint32_t);
    void* data = calloc(1, capacity);
    BinaryWriter binaryWriter = BinaryWriter(data, capacity);

    binaryWriter.Write<uint32_t>(m_RuntimeResourceIDOffsets.size());

    for (auto it = m_RuntimeResourceIDOffsets.begin(); it != m_RuntimeResourceIDOffsets.end(); ++it)
    {
        binaryWriter.Write<uint32_t>(*it);
    }

    return Section(0x578FBCEE, binaryWriter.GetPosition(), binaryWriter.GetBuffer());
}

std::vector<ZBinarySerializer::Section> ZBinarySerializer::GenerateSections()
{
    std::vector<Section> sections;

    if (!m_RebaseLocations.empty())
    {
        sections.push_back(GenerateRebaseLocationsSection());
    }

    if (!m_TypeIDs.empty())
    {
        sections.push_back(GenerateTypeReindexingSection());
    }

    if (!m_RuntimeResourceIDOffsets.empty())
    {
        sections.push_back(GenerateRunimeResourceIDReindexingSection());
    }

    return sections;
}
