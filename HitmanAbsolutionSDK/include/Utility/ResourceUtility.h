#pragma once

#include <Glacier/Resource/ZDynamicResourceLibrary.h>

#include "IO/BinaryReader.h"

class HitmanAbsolutionSDK_API ResourceUtility
{
public:
	static void LoadResource(const unsigned long long resourceRuntimeResourceID, const unsigned long long headerLibraryruntimeResourceID, void*& resourceData, unsigned int& resourceDataSize);
	static void LoadResourceData(BinaryReader& headerLibraryBinaryReader, const unsigned int resourceHeadersStartOffset, const unsigned int indexInLibrary, const std::string& resourceLibraryFilePath, void*& resourceData, unsigned int& resourceDataSize);
	static std::string ConvertResourceIDToFilePath(const std::string& resourceID);
	static std::filesystem::path GetHeaderLibraryPath(const unsigned long long resourceRuntimeResourceID);

	static bool CreateAndInstallDynamicResourceLibrary(ZDynamicResourceLibrary*& dynamicResourceLibrary, const std::string& resourceID, ZRuntimeResourceID& tempRuntimeResourceID, const unsigned int entityCount = 1);
	static bool InstallDynamicResourceLibrary(ZDynamicResourceLibrary*& dynamicResourceLibrary, const std::string& headerLibraryResourceID, ZRuntimeResourceID& tempRuntimeResourceID, const unsigned int entityCount = 1);
	static bool InstallDynamicResourceLibrary(ZDynamicResourceLibrary*& dynamicResourceLibrary, const ZRuntimeResourceID& headerLibraryRuntimeResourceID, ZRuntimeResourceID& tempRuntimeResourceID, const unsigned int entityCount = 1);
	static ZRuntimeResourceID GetTEMPRuntimeResourceID(const ZRuntimeResourceID& headerLibraryRuntimeResourceID);
};
