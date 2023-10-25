#include <Glacier/Serializer/ZBinarySerializer.h>
#include <Glacier/Serializer/BinaryBulkDataHeader.h>
#include <Glacier/Resource/ZRuntimeResourceID.h>
#include <Glacier/Entity/ZVariant.h>
#include <Glacier/TypeInfo/IType.h>

ZBinarySerializer::ZBinarySerializer()
{
	this->buffer = nullptr;
	this->layoutPointer = 0;
	this->capacity = 0;
	this->maxAlignment = 0;
}

unsigned int ZBinarySerializer::ReserveLayoutFor(const unsigned int count, const unsigned int typeSize, const unsigned int typeAlignment, const unsigned int headerSize, const bool useMaxAlignment)
{
	if (typeAlignment > maxAlignment)
	{
		maxAlignment = typeAlignment;
	}

	unsigned int typeAlignment2;

	if (useMaxAlignment)
	{
		typeAlignment2 = maxAlignment;
	}
	else
	{
		typeAlignment2 = typeAlignment;
	}

	unsigned int result = ~(typeAlignment2 - 1) & (typeAlignment2 + layoutPointer - 1);

	if (result - layoutPointer < headerSize)
	{
		result = ~(typeAlignment2 - 1) & (layoutPointer + typeAlignment2 + headerSize - 1);
	}

	layoutPointer = result;

	if (count * typeSize == 0)
	{
		++layoutPointer;
	}
	else
	{
		layoutPointer += count * typeSize;
	}

	return result;
}

unsigned int ZBinarySerializer::GetLayoutPointer()
{
	return layoutPointer;
}

unsigned char ZBinarySerializer::GetMaxAlignment()
{
	return maxAlignment;
}

void ZBinarySerializer::SetLayoutPointer(unsigned int layoutPointer)
{
	this->layoutPointer = layoutPointer;
}

void ZBinarySerializer::RecordOffsetForRebasing(const unsigned int offset)
{
	rebaseLocations.insert(offset);
}

void ZBinarySerializer::RecordOffsetForTypeIDReindexing(const unsigned int offset)
{
	typeReindexLocations.push_back(offset);
}

void ZBinarySerializer::RecordRuntimeResourceIDOffset(const unsigned int offset)
{
	runtimeResourceIDOffsets.insert(offset);
}

unsigned int ZBinarySerializer::TypeIdToIndex(STypeID* typeID)
{
	unsigned int typeIndex = typeIDs.size();

	for (unsigned int i = 0; i < typeIDs.size(); i++)
	{
		if (typeIDs[i] == typeID)
		{
			typeIndex = i;

			break;
		}
	}

	if (typeIndex == typeIDs.size())
	{
		typeIDs.push_back(typeID);
	}

	return typeIndex;
}

void ZBinarySerializer::WriteToFileStream(const std::string& filePath, std::unordered_map<unsigned long long, unsigned int>* references)
{
	void* data = nullptr;
	unsigned int dataSize = 0;

	WriteToMemoryStream(&data, dataSize, references);

	BinaryWriter binaryWriter = BinaryWriter(filePath);

	binaryWriter.Write(data, dataSize);
}

void ZBinarySerializer::WriteToMemoryStream(void** data, unsigned int& dataSize, std::unordered_map<unsigned long long, unsigned int>* references)
{
	std::vector<ZBinarySerializer::Section> sections = GenerateSections();
	int sizeOfAllSections = 0;

	for (unsigned int i = 0; i < sections.size(); i++)
	{
		sizeOfAllSections += sections[i].size;
	}

	sizeOfAllSections += sections.size() * 4;

	uintptr_t capacity = 16 + layoutPointer + sizeOfAllSections;

	*data = calloc(1, capacity);

	BinaryWriter binaryWriter = BinaryWriter(*data, capacity);

	binaryWriter.Write<unsigned int>(0x324E4942);
	binaryWriter.Write<unsigned char>(BinaryBulkDataHeader::EDataEndianness::FLAG_MASK_LITTLENDIAN);
	binaryWriter.Write<unsigned char>(maxAlignment);
	binaryWriter.Write<unsigned char>(static_cast<unsigned char>(sections.size()));
	binaryWriter.Write<unsigned char>(0);

	const unsigned char dataLength0 = (layoutPointer & 0xFF000000u) >> 24;
	const unsigned char dataLength1 = (layoutPointer & 0xFF0000) >> 16;
	const unsigned char dataLength2 = (layoutPointer & 0xFF00) >> 8;
	const unsigned char dataLength3 = layoutPointer & 0xFFu;

	binaryWriter.Write<unsigned char>(dataLength0);
	binaryWriter.Write<unsigned char>(dataLength1);
	binaryWriter.Write<unsigned char>(dataLength2);
	binaryWriter.Write<unsigned char>(dataLength3);

	binaryWriter.Write<unsigned int>(0);

	unsigned int previousRebaseLocation = 0;
	BinaryWriter rebaseSectionBinaryWriter = BinaryWriter(sections[0].data, sections[0].size);

	rebaseSectionBinaryWriter.Seek(4, SeekOrigin::Begin);

	for (auto it = rebaseLocations.begin(); it != rebaseLocations.end(); ++it)
	{
		const unsigned int rebaseLocation = *it;
		unsigned int value = *reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(buffer) + rebaseLocation);

		if (value == 0)
		{
			--value;
		}

		value -= rebaseLocation;

		WriteToMemory(&value, sizeof(unsigned int), rebaseLocation);

		rebaseSectionBinaryWriter.Write<unsigned int>(rebaseLocation - previousRebaseLocation);

		previousRebaseLocation = rebaseLocation;
	}

	if (!typeIDs.empty())
	{
		unsigned int previousTypeReindexLocation = 0;
		BinaryWriter typeReindexingSectionBinaryWriter = BinaryWriter(sections[1].data, sections[1].size);

		typeReindexingSectionBinaryWriter.Seek(4, SeekOrigin::Begin);

		for (unsigned int i = 0; i < typeReindexLocations.size(); ++i)
		{
			const unsigned int typeReindexLocation = typeReindexLocations[i];

			typeReindexingSectionBinaryWriter.Write<unsigned int>(typeReindexLocation - previousTypeReindexLocation);

			previousTypeReindexLocation = typeReindexLocation;
		}
	}

	if (!runtimeResourceIDOffsets.empty())
	{
		BinaryWriter runimeResourceIDReindexingSectionBinaryWriter = BinaryWriter(sections[2].data, sections[2].size);

		runimeResourceIDReindexingSectionBinaryWriter.Seek(4, SeekOrigin::Begin);

		for (auto it = runtimeResourceIDOffsets.begin(); it != runtimeResourceIDOffsets.end(); ++it)
		{
			const unsigned int offset = *it;
			const ZRuntimeResourceID runtimeResourceID = *reinterpret_cast<ZRuntimeResourceID*>(reinterpret_cast<uintptr_t>(buffer) + offset);

			if (runtimeResourceID.GetID() == 0xFFFFFFFFFFFFFFFF)
			{
				*reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(buffer) + offset) = 0xFFFFFFFF;
				*reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(buffer) + offset + 4) = 0xFFFFFFFF;
			}
			else
			{
				const unsigned int index = references->operator[](runtimeResourceID.GetID());

				*reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(buffer) + offset) = 0;
				*reinterpret_cast<unsigned int*>(reinterpret_cast<uintptr_t>(buffer) + offset + 4) = index;
			}

			runimeResourceIDReindexingSectionBinaryWriter.Write<unsigned int>(offset);
		}
	}

	binaryWriter.Write(buffer, layoutPointer);

	for (unsigned int i = 0; i < sections.size(); i++)
	{
		binaryWriter.Write<unsigned int>(sections[i].type);
		binaryWriter.Write<unsigned int>(sections[i].size);
		binaryWriter.Write(sections[i].data, sections[i].size);
	}

	*data = binaryWriter.GetBuffer();
	dataSize = binaryWriter.GetPosition();
}

void ZBinarySerializer::EnsureCapacity(const unsigned int size)
{
	if (size > capacity)
	{
		unsigned int newCapacity = capacity * 2;

		while (newCapacity < size)
		{
			newCapacity *= 2;
		}

		buffer = realloc(buffer, newCapacity);

		unsigned int size2 = newCapacity - capacity;
		void* buffer2 = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(buffer) + capacity);

		memset(buffer2, 0, size2);

		capacity = newCapacity;
	}
}

void ZBinarySerializer::WriteToMemory(const void* data, const unsigned int bytes, const unsigned int offset)
{
	EnsureCapacity(bytes + offset);

	void* buffer2 = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(buffer) + offset);

	memcpy(buffer2, data, bytes);
}

unsigned int ZBinarySerializer::GetAlignedOffset(const unsigned int typeSize, const unsigned int typeAlignment)
{
	if (typeAlignment > maxAlignment)
	{
		maxAlignment = typeAlignment;
	}

	unsigned int result = ~(typeAlignment - 1) & (layoutPointer + typeAlignment - 1);

	layoutPointer = result;

	if (!typeSize)
	{
		++layoutPointer;
	}
	else
	{
		layoutPointer += typeSize;
	}

	return result;
}

unsigned int ZBinarySerializer::GetAlignedLayoutPointer(const unsigned int layoutPointer, const unsigned int alignment)
{
	return ~(alignment - 1) & (layoutPointer + alignment - 1);
}

std::optional<unsigned int> ZBinarySerializer::GetDataOffset(const ZVariant* variant)
{
	auto it = typeIDs2.find(variant->GetTypeID());

	if (it == typeIDs2.end())
	{
		return std::nullopt;
	}

	auto& dataPointers = it->second;

	for (auto it2 = dataPointers.begin(); it2 != dataPointers.end(); ++it2)
	{
		if (variant->GetData() == it2->first)
		{
			return std::make_optional(it2->second);
		}
	}

	return std::nullopt;
}

void ZBinarySerializer::SetDataOffset(const ZVariant* variant, unsigned int offset)
{
	auto it = typeIDs2.find(variant->GetTypeID());

	if (it != typeIDs2.end())
	{
		it->second[variant->GetData()] = offset;

		return;
	}

	std::unordered_map<void*, unsigned int> dataPointers;

	dataPointers[variant->GetData()] = offset;
	typeIDs2[variant->GetTypeID()] = dataPointers;
}

ZBinarySerializer::Section ZBinarySerializer::GenerateRebaseLocationsSection()
{
	unsigned int capacity = (rebaseLocations.size() + 1) * sizeof(unsigned int);
	void* data = calloc(1, capacity);
	BinaryWriter binaryWriter = BinaryWriter(data, capacity);

	binaryWriter.Write<unsigned int>(rebaseLocations.size());

	for (auto it = rebaseLocations.begin(); it != rebaseLocations.end(); ++it)
	{
		binaryWriter.Write<unsigned int>(*it);
	}

	return Section(0x12EBA5ED, binaryWriter.GetPosition(), binaryWriter.GetBuffer());
}

ZBinarySerializer::Section ZBinarySerializer::GenerateTypeReindexingSection()
{
	unsigned int capacity = (typeReindexLocations.size() + typeIDs.size() + 2) * sizeof(unsigned int);
	void* data = calloc(1, capacity);
	BinaryWriter binaryWriter = BinaryWriter(data, capacity);

	binaryWriter.Write<unsigned int>(typeReindexLocations.size());

	for (unsigned int i = 0; i < typeReindexLocations.size(); ++i)
	{
		binaryWriter.Write<unsigned int>(typeReindexLocations[i]);
	}

	binaryWriter.Write<unsigned int>(typeIDs.size());

	for (unsigned int i = 0; i < typeIDs.size(); i++)
	{
		binaryWriter.AlignTo(4);

		binaryWriter.Write<unsigned int>(i);
		binaryWriter.Write<int>(-1);
		binaryWriter.Write<unsigned int>(strlen(typeIDs[i]->pTypeInfo->GetTypeName()) + 1);
		binaryWriter.WriteString(typeIDs[i]->pTypeInfo->GetTypeName());
	}

	return Section(0x3989BF9F, binaryWriter.GetPosition(), binaryWriter.GetBuffer());
}

ZBinarySerializer::Section ZBinarySerializer::GenerateRunimeResourceIDReindexingSection()
{
	unsigned int capacity = (runtimeResourceIDOffsets.size() + 1) * sizeof(unsigned int);
	void* data = calloc(1, capacity);
	BinaryWriter binaryWriter = BinaryWriter(data, capacity);

	binaryWriter.Write<unsigned int>(runtimeResourceIDOffsets.size());

	for (auto it = runtimeResourceIDOffsets.begin(); it != runtimeResourceIDOffsets.end(); ++it)
	{
		binaryWriter.Write<unsigned int>(*it);
	}

	return Section(0x578FBCEE, binaryWriter.GetPosition(), binaryWriter.GetBuffer());
}

std::vector<ZBinarySerializer::Section> ZBinarySerializer::GenerateSections()
{
	std::vector<Section> sections;

	if (!rebaseLocations.empty())
	{
		sections.push_back(GenerateRebaseLocationsSection());
	}

	if (!typeIDs.empty())
	{
		sections.push_back(GenerateTypeReindexingSection());
	}

	if (!runtimeResourceIDOffsets.empty())
	{
		sections.push_back(GenerateRunimeResourceIDReindexingSection());
	}

	return sections;
}

ZBinarySerializer::Section::Section(unsigned int type, unsigned int size, void* data)
{
	this->type = type;
	this->size = size;
	this->data = data;
}
