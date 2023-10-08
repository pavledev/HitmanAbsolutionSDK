#pragma once

#include "IO/BinaryReader.h"

class HitmanAbsolutionSDK_API ResourceUtility
{
public:
	static void LoadResource(const unsigned long long resourceRuntimeResourceID, const unsigned long long headerLibraryruntimeResourceID, void*& resourceData, unsigned int& resourceDataSize);
	static void LoadResourceData(BinaryReader& headerLibraryBinaryReader, const unsigned int resourceHeadersStartOffset, const unsigned int indexInLibrary, const std::string& resourceLibraryFilePath, void*& resourceData, unsigned int& resourceDataSize);
	static std::string ConvertResourceIDToFilePath(const std::string& resourceID);
};
