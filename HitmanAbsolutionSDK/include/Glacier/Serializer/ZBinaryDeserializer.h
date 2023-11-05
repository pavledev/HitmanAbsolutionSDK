#pragma once

#include "../TypeInfo/STypeID.h"

#include <IO/BinaryReader.h>
#include <IO/BinaryWriter.h>

class HitmanAbsolutionSDK_API ZBinaryDeserializer
{
public:
	ZBinaryDeserializer();
	void* Deserialize(const std::string& filePath);
	void* Deserialize(void* buffer, const size_t size);
	void* Deserialize(BinaryReader& binaryReader);
	const unsigned char GetAlignment() const;

private:
	void ParseRebaseSection(BinaryReader& binaryReader, BinaryReader& dataSectionbinaryReader, BinaryWriter& dataSectionBinaryWriter);

	unsigned char alignment;
};
