#pragma once

#include <iostream>
#include <cstdint>
#include <cassert>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <optional>
#include <set>

#include <IO/BinaryReader.h>
#include <IO/BinaryWriter.h>

#include <Logging.h>

class ZVariant;
struct STypeID;

struct BinaryBulkDataHeader
{
    enum EDataEndianness
    {
        FLAG_MASK_LITTLENDIAN,
        FLAG_MASK_BIGENDIAN,
    };
};

class ZBinaryDeserializer
{
  public:
    uint8_t GetAlignment()
    {
        return m_Alignment;
    }

    void* Deserialize(const std::string& p_FilePath)
    {
        BinaryReader binaryReader = BinaryReader(p_FilePath);

        return Deserialize(binaryReader);
    }

    void* Deserialize(void* p_Buffer, const size_t p_Size)
    {
        BinaryReader binaryReader = BinaryReader(p_Buffer, p_Size);

        return Deserialize(binaryReader);
    }

    void* Deserialize(BinaryReader& p_BinaryReader)
    {
        uint32_t magic = p_BinaryReader.Read<uint32_t>();

        if (magic != '2NIB')
        {
            Logger::Error("File format not supported!");

            return nullptr;
        }

        BinaryBulkDataHeader::EDataEndianness dataEndianness = static_cast<BinaryBulkDataHeader::EDataEndianness>(p_BinaryReader.Read<uint8_t>());
        const uint8_t alignment = p_BinaryReader.Read<uint8_t>();
        const uint8_t sectionsCount = p_BinaryReader.Read<uint8_t>();
        const uint8_t unusedByte = p_BinaryReader.Read<uint8_t>();

        const uint8_t dataLength0 = p_BinaryReader.Read<uint8_t>();
        const uint8_t dataLength1 = p_BinaryReader.Read<uint8_t>();
        const uint8_t dataLength2 = p_BinaryReader.Read<uint8_t>();
        const uint8_t dataLength3 = p_BinaryReader.Read<uint8_t>();
        const uint32_t dataLength = (dataLength0 << 24) + (dataLength1 << 16) + (dataLength2 << 8) + dataLength3;

        uint32_t unusedDWORD = p_BinaryReader.Read<uint32_t>();

        void* data = operator new(dataLength, std::align_val_t(alignment));

        p_BinaryReader.Read(data, dataLength);

        BinaryReader dataSectionBinaryReader(data, dataLength);
        BinaryWriter dataSectionBinaryWriter(data, dataLength);

        for (uint8_t i = 0; i < sectionsCount; ++i)
        {
            const uint32_t sectionType = p_BinaryReader.Read<uint32_t>();
            const uint32_t sectionSize = p_BinaryReader.Read<uint32_t>();

            switch (sectionType)
            {
            case 0x12EBA5ED:
                ParseRebaseSection(p_BinaryReader, dataSectionBinaryReader, dataSectionBinaryWriter);
                break;
                /*case 0x3989BF9F:
                    ParseTypeReindexingSection(p_BinaryReader, dataSectionBinaryReader, dataSectionBinaryWriter);
                    break;
                case 0x578FBCEE:
                    ParseRuntimeResourceIDReindexingSection(p_BinaryReader, dataSectionBinaryReader, dataSectionBinaryWriter, references);
                    break;
                default:
                {
                    std::stringstream stream;

                    stream << std::hex << sectionType;

                    Logger::GetInstance().Log(Logger::Level::Error, "Unknown section type: 0x{}!", stream.str());

                    return nullptr;
                }*/
            }
        }

        this->m_Alignment = alignment;

        return data;
    }

  private:
    void ParseRebaseSection(BinaryReader& p_BinaryReader, BinaryReader& p_DataSectionbinaryReader, BinaryWriter& p_DataSectionBinaryWriter)
    {
        const uint32_t numberOfRebaseLocations = p_BinaryReader.Read<uint32_t>();
        uint32_t rebaseLocationOffset = 0;

        for (uint32_t i = 0; i < numberOfRebaseLocations; ++i)
        {
            rebaseLocationOffset += p_BinaryReader.Read<uint32_t>();

            p_DataSectionbinaryReader.Seek(rebaseLocationOffset, SeekOrigin::Begin);
            p_DataSectionBinaryWriter.Seek(rebaseLocationOffset, SeekOrigin::Begin);

            intptr_t value = p_DataSectionbinaryReader.Read<uint32_t>() + rebaseLocationOffset;

            if (value != -1)
            {
                p_DataSectionBinaryWriter.Write(reinterpret_cast<uintptr_t>(p_DataSectionbinaryReader.GetBuffer()) + value);
            }
            else
            {
                p_DataSectionBinaryWriter.Write(0);
            }
        }
    }

    uint8_t m_Alignment = 0;
};

class ZBinarySerializer
{
  public:
    struct Section
    {
        Section(uint32_t p_Type, uint32_t p_Size, void* p_Data) : m_Type(p_Type), m_Size(p_Size), m_Data(p_Data) {}

        uint32_t m_Type;
        uint32_t m_Size;
        void* m_Data;
    };

    ZBinarySerializer();

    template<typename T>
    void SerializeToMemory(T* p_Type, void** p_Data, uint32_t& p_DataSize, std::unordered_map<uint64_t, uint32_t>* p_References = nullptr)
    {
        m_Buffer = calloc(1, sizeof(T));
        m_LayoutPointer += sizeof(T);
        m_Capacity += sizeof(T);

        p_Type->SerializeToMemory(*this);

        WriteToMemoryStream(p_Data, p_DataSize, p_References);
    }

    template<typename T>
    void SerializeToFile(T* p_Type, const std::string& p_FilePath, std::unordered_map<uint64_t, uint32_t>* p_References = nullptr)
    {
        m_Buffer = calloc(1, sizeof(T));
        m_LayoutPointer += sizeof(T);
        m_Capacity += sizeof(T);

        p_Type->SerializeToMemory(*this);

        WriteToFileStream(p_FilePath, p_References);

        free(m_Buffer);
    }

    uint8_t GetMaxAlignment();

    uint32_t GetLayoutPointer();
    uint32_t ReserveLayoutFor(uint32_t p_Count, uint32_t p_TypeSize, uint32_t p_TypeAlignment, uint32_t p_HeaderSize, bool p_UseMaxAlignment = false);
    void SetLayoutPointer(uint32_t p_LayoutPointer);

    void RecordOffsetForRebasing(uint32_t p_Offset);
    void RecordOffsetForTypeIDReindexing(uint32_t p_Offset);
    void RecordRuntimeResourceIDOffset(uint32_t p_Offset);

    uint32_t TypeIdToIndex(STypeID* p_TypeID);

    void WriteToFileStream(const std::string& p_FilePath, std::unordered_map<uint64_t, uint32_t>* p_References = nullptr);
    void WriteToMemoryStream(void** p_Data, uint32_t& p_DataSize, std::unordered_map<uint64_t, uint32_t>* p_References = nullptr);
    void WriteToMemory(const void* p_Data, uint32_t p_Size, uint32_t p_Offset);

    void EnsureCapacity(uint32_t p_Size);

    uint32_t GetAlignedOffset(uint32_t p_TypeSize, uint32_t p_TypeAlignment);
    uint32_t GetAlignedLayoutPointer(uint32_t p_LayoutPointer, uint32_t p_Alignment);

    std::optional<uint32_t> GetDataOffset(const ZVariant* p_Variant);
    void SetDataOffset(const ZVariant* p_Variant, uint32_t p_Offset);

    Section GenerateRebaseLocationsSection();
    Section GenerateTypeReindexingSection();
    Section GenerateRunimeResourceIDReindexingSection();
    std::vector<Section> GenerateSections();

  private:
    void* m_Buffer;
    uint32_t m_LayoutPointer;
    uint32_t m_Capacity;
    std::set<uint32_t> m_RebaseLocations;
    std::vector<STypeID*> m_TypeIDs;
    std::vector<uint32_t> m_TypeReindexLocations;
    std::set<uint32_t> m_RuntimeResourceIDOffsets;
    std::unordered_map<STypeID*, std::unordered_map<void*, uint32_t>> m_VariantDataOffsets;
    uint8_t m_MaxAlignment;
};
