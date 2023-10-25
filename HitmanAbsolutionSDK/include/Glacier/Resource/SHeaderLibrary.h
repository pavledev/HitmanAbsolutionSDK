#pragma once

#include "SHeaderLibraryChunk.h"

class ZBinarySerializer;

struct SHeaderLibrary
{
	void SerializeToMemory(ZBinarySerializer& binarySerializer);

	TArray<SHeaderLibraryChunk> chunks;
	TArray<ZString> externalResourceIds;
	long long ridSource;
};
