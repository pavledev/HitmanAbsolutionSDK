#pragma once

#include "../Templates/TArray.h"
#include "../ZString.h"

class ZBinarySerializer;

struct SHeaderLibraryChunk
{
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);

	ZString sLibraryID;
	unsigned int nOffset;
	char nFlags;
	unsigned int localizationCategory;
	TArray<ZString> sLanguages;
	TArray<int> states;
	TArray<TArray<unsigned char>> resourceHeaders;
	TArray<unsigned int> ridMappingIndices;
	TArray<unsigned long long> ridMappingIDs;
};
