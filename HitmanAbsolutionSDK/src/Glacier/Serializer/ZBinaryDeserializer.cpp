#include <sstream>

#include <Glacier/Serializer/ZBinaryDeserializer.h>
#include <Glacier/Serializer/BinaryBulkDataHeader.h>

#include "Logger.h"

ZBinaryDeserializer::ZBinaryDeserializer()
{
	alignment = 0;
}

void* ZBinaryDeserializer::Deserialize(const std::string& filePath)
{
	BinaryReader binaryReader = BinaryReader(filePath);

	return Deserialize(binaryReader);
}

void* ZBinaryDeserializer::Deserialize(void* buffer, const size_t size)
{
	BinaryReader binaryReader = BinaryReader(buffer, size);

	return Deserialize(binaryReader);
}

void* ZBinaryDeserializer::Deserialize(BinaryReader& binaryReader)
{
	unsigned int magic = binaryReader.Read<unsigned int>();

	if (magic != '2NIB')
	{
		Logger::GetInstance().Log(Logger::Level::Error, "File format not supported!");

		return nullptr;
	}

	BinaryBulkDataHeader::EDataEndianness dataEndianness = static_cast<BinaryBulkDataHeader::EDataEndianness>(binaryReader.Read<unsigned char>());
	const unsigned char alignment = binaryReader.Read<unsigned char>();
	const unsigned char sectionsCount = binaryReader.Read<unsigned char>();
	const unsigned char unusedByte = binaryReader.Read<unsigned char>();

	const unsigned char dataLength0 = binaryReader.Read<unsigned char>();
	const unsigned char dataLength1 = binaryReader.Read<unsigned char>();
	const unsigned char dataLength2 = binaryReader.Read<unsigned char>();
	const unsigned char dataLength3 = binaryReader.Read<unsigned char>();
	const unsigned int dataLength = (dataLength0 << 24) + (dataLength1 << 16) + (dataLength2 << 8) + dataLength3;

	unsigned int unusedDWORD = binaryReader.Read<unsigned int>();

	void* data = operator new(dataLength, std::align_val_t(alignment));

	binaryReader.Read(data, dataLength);

	BinaryReader dataSectionBinaryReader(data, dataLength);
	BinaryWriter dataSectionBinaryWriter(data, dataLength);

	for (unsigned char i = 0; i < sectionsCount; ++i)
	{
		const unsigned int sectionType = binaryReader.Read<unsigned int>();
		const unsigned int sectionSize = binaryReader.Read<unsigned int>();

		switch (sectionType)
		{
			case 0x12EBA5ED:
				ParseRebaseSection(binaryReader, dataSectionBinaryReader, dataSectionBinaryWriter);
				break;
			/*case 0x3989BF9F:
				ParseTypeReindexingSection(binaryReader, dataSectionBinaryReader, dataSectionBinaryWriter);
				break;
			case 0x578FBCEE:
				ParseRuntimeResourceIDReindexingSection(binaryReader, dataSectionBinaryReader, dataSectionBinaryWriter, references);
				break;*/
			default:
			{
				std::stringstream stream;

				stream << std::hex << sectionType;

				Logger::GetInstance().Log(Logger::Level::Error, "Unknown section type: 0x{}!", stream.str());

				return nullptr;
			}
		}
	}

	this->alignment = alignment;

	return data;
}

const unsigned char ZBinaryDeserializer::GetAlignment() const
{
	return alignment;
}

void ZBinaryDeserializer::ParseRebaseSection(BinaryReader& binaryReader, BinaryReader& dataSectionbinaryReader, BinaryWriter& dataSectionBinaryWriter)
{
	const unsigned int numberOfRebaseLocations = binaryReader.Read<unsigned int>();
	unsigned int rebaseLocationOffset = 0;

	for (unsigned int i = 0; i < numberOfRebaseLocations; ++i)
	{
		rebaseLocationOffset += binaryReader.Read<unsigned int>();

		dataSectionbinaryReader.Seek(rebaseLocationOffset, SeekOrigin::Begin);
		dataSectionBinaryWriter.Seek(rebaseLocationOffset, SeekOrigin::Begin);

		intptr_t value = dataSectionbinaryReader.Read<unsigned int>() + rebaseLocationOffset;

		if (value != -1)
		{
			dataSectionBinaryWriter.Write(reinterpret_cast<uintptr_t>(dataSectionbinaryReader.GetBuffer()) + value);
		}
		else
		{
			dataSectionBinaryWriter.Write(0);
		}
	}
}
