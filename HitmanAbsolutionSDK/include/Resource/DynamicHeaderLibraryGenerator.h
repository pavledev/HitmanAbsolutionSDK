#pragma once

#include <set>
#include <map>
#include <unordered_map>

#include <Glacier/Resource/ZRuntimeResourceID.h>
#include <Glacier/Resource/SHeaderLibrary.h>

class HitmanAbsolutionSDK_API DynamicHeaderLibraryGenerator
{
public:
	~DynamicHeaderLibraryGenerator();
	void ExtractResourceToHeaderLibrary(const std::string& resourceID, const std::string& headerLibraryFilePath, std::string& newHeaderLibraryResourceID);

private:
	void AddReferencesToChunks(TArray<unsigned char>* resourceHeader, const ZRuntimeResourceID& runtimeResourceID);
	void UpdateReferenceRuntimeResourceIDs(const std::string& resourceID);
	std::string GenerateHeaderLibraryResourceID(const std::string& resourceID);
	std::string GenerateHeaderLibraryChunkResourceID(const std::string& resourceID, const unsigned int index);
	void PadTo(std::string& str, const size_t num, const char paddingChar = '0');
	void UpdateHeaderLibraryChunksData(const std::string& resourceID);
	void CreateHeaderLibrary(const std::string& resourceID, const std::string& headerLibraryPath, std::string& newHeaderLibraryResourceID);
	void CreateResourceLibrary(const std::string& oldChunkResourceID, const std::string& newChunkResourceID,
		std::map<unsigned int, std::map<unsigned int, unsigned int>>& oldIndicesToOldOffsets, const std::map<unsigned int, unsigned int>& newIndicesToOldIndices);
	void CreateResourceLibraries();

	std::string gamePath;
	ZRuntimeResourceID sourceResourceRuntimeResourceID;
	SHeaderLibrary* headerLibrary;
	std::unordered_map<unsigned int, unsigned int> chunkHashesToIndices;
	std::unordered_map<unsigned int, std::string> chunkHashesToResourceIDs;
	std::map<std::string, SHeaderLibraryChunk> newChunks;
	std::map<std::string, std::set<unsigned long long>> chunkResources;
	std::map<std::string, std::map<unsigned int, unsigned int>> oldToNewIndices;
	std::map<std::string, std::string> oldChunkResourceIDsToNewResourceIDs;
	std::unordered_map<std::string, unsigned long long> resourceIDsToHashes;
	std::set<std::string> globalResourceIDs;
	std::string newFolderPath;
	std::vector<std::string> generatedHeaderAndResourceLibraryFilePaths;
	bool folderAlreadyExists;
};
