#pragma once

#include <set>
#include <map>
#include <unordered_map>

#include <Glacier/ZResource.h>

class DynamicHeaderLibraryGenerator
{
  public:
    ~DynamicHeaderLibraryGenerator();

    void ExtractResourceToHeaderLibrary(
        const std::string& p_ResourceID, const std::string& p_HeaderLibraryFilePath, std::string& p_NewHeaderLibraryResourceID
    );

  private:
    void AddReferencesToChunks(TArray<uint8_t>* p_ResourceHeader, const ZRuntimeResourceID& p_RuntimeResourceID);
    void UpdateReferenceRuntimeResourceIDs();
    std::string GenerateHeaderLibraryResourceID(const std::string& p_ResourceID);
    std::string GenerateHeaderLibraryChunkResourceID(const std::string& p_ResourceID, uint32_t p_Index);
    void PadLeft(std::string& p_String, size_t p_TargetLength, char p_PaddingChar = '0');
    void UpdateHeaderLibraryChunksData();
    void CreateHeaderLibrary(const std::string& p_ResourceID, std::string& p_NewHeaderLibraryResourceID);
    void CreateResourceLibrary(
        const std::string& p_OldChunkResourceID, const std::string& p_NewChunkResourceID,
        std::map<uint32_t, std::map<uint32_t, uint32_t>>& p_OldIndicesToOldOffsetsAndSizes,
        const std::map<uint32_t, uint32_t>& p_NewIndicesToOldIndices
    );
    void CreateResourceLibraries();

    ZRuntimeResourceID m_SourceResourceRuntimeResourceID;
    SHeaderLibrary* m_HeaderLibrary = nullptr;
    std::unordered_map<uint32_t, uint32_t> m_ChunkHashesToIndices;
    std::unordered_map<uint32_t, std::string> m_ChunkHashesToResourceIDs;
    std::map<std::string, SHeaderLibraryChunk> m_NewChunks;
    std::map<std::string, std::set<uint64_t>> m_ChunkResources;
    std::map<std::string, std::map<uint32_t, uint32_t>> m_OldToNewIndices;
    std::map<std::string, std::string> m_OldChunkResourceIDsToNewResourceIDs;
    std::unordered_map<std::string, uint64_t> m_ResourceIDsToHashes;
    std::set<std::string> m_GlobalResourceIDs;
    std::string m_NewFolderPath;
    std::vector<std::string> m_GeneratedHeaderAndResourceLibraryFilePaths;
    bool m_FolderAlreadyExists = false;
};
