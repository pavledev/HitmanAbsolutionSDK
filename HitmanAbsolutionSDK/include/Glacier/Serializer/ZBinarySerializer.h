#pragma once

#include <iostream>
#include <cstdint>
#include <cassert>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <optional>
#include <set>

#include "../TypeInfo/STypeID.h"

#include <IO/BinaryWriter.h>

class ZVariant;

class ZBinarySerializer
{
public:
	struct Section
	{
		unsigned int type;
		unsigned int size;
		void* data;

		Section(unsigned int type, unsigned int size, void* data);
	};

	ZBinarySerializer();

	template <typename T>
	void SerializeToMemory(T* type, void** data, unsigned int& dataSize, std::unordered_map<unsigned long long, unsigned int>* references = nullptr)
	{
		buffer = calloc(1, sizeof(T));
		layoutPointer += sizeof(T);
		capacity += sizeof(T);

		type->SerializeToMemory(*this);

		WriteToMemoryStream(data, dataSize, references);
	}

	template <typename T>
	void SerializeToFile(T* type, const std::string& filePath, std::unordered_map<unsigned long long, unsigned int>* references = nullptr)
	{
		buffer = calloc(1, sizeof(T));
		layoutPointer += sizeof(T);
		capacity += sizeof(T);

		type->SerializeToMemory(*this);

		WriteToFileStream(filePath, references);

		free(buffer);
	}

	unsigned int ReserveLayoutFor(const unsigned int count, const unsigned int typeSize, const unsigned int typeAlignment, const unsigned int headerSize, const bool useMaxAlignment = false);
	unsigned int GetLayoutPointer();
	unsigned char GetMaxAlignment();
	void SetLayoutPointer(unsigned int layoutPointer);
	void RecordOffsetForRebasing(const unsigned int offset);
	void RecordOffsetForTypeIDReindexing(const unsigned int offset);
	void RecordRuntimeResourceIDOffset(const unsigned int offset);
	unsigned int TypeIdToIndex(STypeID* typeID);
	void WriteToFileStream(const std::string& filePath, std::unordered_map<unsigned long long, unsigned int>* references = nullptr);
	void WriteToMemoryStream(void** data, unsigned int& dataSize, std::unordered_map<unsigned long long, unsigned int>* references = nullptr);
	void EnsureCapacity(const unsigned int size);
	void WriteToMemory(const void* data, const unsigned int size, const unsigned int offset);
	unsigned int GetAlignedOffset(const unsigned int typeSize, const unsigned int typeAlignment);
	unsigned int GetAlignedLayoutPointer(const unsigned int layoutPointer, const unsigned int alignment);
	std::optional<unsigned int> GetDataOffset(const ZVariant* variant);
	void SetDataOffset(const ZVariant* variant, unsigned int offset);
	Section GenerateRebaseLocationsSection();
	Section GenerateTypeReindexingSection();
	Section GenerateRunimeResourceIDReindexingSection();
	std::vector<Section> GenerateSections();

private:
	void* buffer;
	unsigned int layoutPointer;
	unsigned int capacity;
	std::set<unsigned int> rebaseLocations;
	std::vector<STypeID*> typeIDs;
	std::vector<unsigned int> typeReindexLocations;
	std::set<unsigned int> runtimeResourceIDOffsets;
	std::unordered_map<STypeID*, std::unordered_map<void*, unsigned int>> typeIDs2;
	unsigned char maxAlignment;
};
