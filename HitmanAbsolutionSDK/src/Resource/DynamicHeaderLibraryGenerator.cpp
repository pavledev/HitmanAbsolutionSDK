#include <filesystem>

#include <Glacier/ZSerializer.h>
#include <Glacier/ZResourceHeaderReader.h>

#include <Resource/DynamicHeaderLibraryGenerator.h>
#include <Registry/ResourceIDRegistry.h>
#include <Utils/ResourceUtils.h>
#include <Hash.h>
#include <Utils/StringUtils.h>

DynamicHeaderLibraryGenerator::~DynamicHeaderLibraryGenerator()
{
    if (m_FolderAlreadyExists)
    {
        for (size_t i = 0; i < m_GeneratedHeaderAndResourceLibraryFilePaths.size(); ++i)
        {
            std::filesystem::remove(m_GeneratedHeaderAndResourceLibraryFilePaths[i]);
        }
    }
    else
    {
        std::filesystem::remove_all(m_NewFolderPath);
    }
}

void DynamicHeaderLibraryGenerator::ExtractResourceToHeaderLibrary(
    const std::string& p_ResourceID, const std::string& p_HeaderLibraryFilePath, std::string& p_NewHeaderLibraryResourceID
)
{
    std::string resourceID;

    if (p_ResourceID.contains("aspectdummy"))
    {
        const size_t index = p_ResourceID.find("(");

        resourceID = p_ResourceID.substr(index + 1, p_ResourceID.find(",") - index - 1);
    }
    else
    {
        resourceID = p_ResourceID;
    }

    if (resourceID.contains(" "))
    {
        util::ReplaceAll(resourceID, " ", "");
    }

    m_SourceResourceRuntimeResourceID = SDK().GetRuntimeResourceID(p_ResourceID.c_str());

    ZBinaryDeserializer binaryDeserializer;
    BinaryReader headerBinaryReader = BinaryReader(p_HeaderLibraryFilePath);

    headerBinaryReader.Seek(4, SeekOrigin::Begin);

    uint32_t referencesChunkSize = headerBinaryReader.Read<uint32_t>();

    headerBinaryReader.Seek(referencesChunkSize + 24, SeekOrigin::Begin);

    m_HeaderLibrary = static_cast<SHeaderLibrary*>(binaryDeserializer.Deserialize(headerBinaryReader));

    for (uint32_t i = 0; i < m_HeaderLibrary->chunks.Size(); ++i)
    {
        std::string resourceID2 = m_HeaderLibrary->chunks[i].sLibraryID.ToCString();
        std::string resourceLibraryFilePath = util::ConvertResourceIDToFilePath(resourceID2);

        if (!std::filesystem::exists(resourceLibraryFilePath))
        {
            continue;
        }

        hash::MD5Hash md5Hash = hash::MD5(util::ToLowerCase(resourceID2));
        uint32_t hash = std::strtoul(hash::MD5ToString(md5Hash).substr(8, 8).c_str(), nullptr, 16);

        m_ChunkHashesToIndices[hash] = i;
        m_ChunkHashesToResourceIDs[hash] = resourceID2;
    }

    const auto chunkIt = m_ChunkHashesToIndices.find(m_SourceResourceRuntimeResourceID.m_IDLow);

    if (chunkIt == m_ChunkHashesToIndices.end())
    {
        Logger::Error("Failed to find chunk for resource '{}'.", p_ResourceID);
        operator delete(m_HeaderLibrary, std::align_val_t(binaryDeserializer.GetAlignment()));

        return;
    }

    const uint32_t chunkIndex = chunkIt->second;
    const uint32_t indexInLibrary = m_SourceResourceRuntimeResourceID.GetIndexInLibrary();

    SHeaderLibraryChunk* headerLibraryChunk = &m_HeaderLibrary->chunks[chunkIndex];

    if (indexInLibrary >= headerLibraryChunk->resourceHeaders.Size())
    {
        Logger::Error(
            "Resource index {} is out of range for '{}' ({} resource headers).", indexInLibrary, p_ResourceID,
            headerLibraryChunk->resourceHeaders.Size()
        );

        operator delete(m_HeaderLibrary, std::align_val_t(binaryDeserializer.GetAlignment()));

        return;
    }

    const std::string chunkResourceID = headerLibraryChunk->sLibraryID.ToCString();
    TArray<unsigned char>* resourceHeader = &headerLibraryChunk->resourceHeaders[indexInLibrary];

    AddReferencesToChunks(resourceHeader, m_SourceResourceRuntimeResourceID);

    const uint32_t newIndexInLibrary = m_NewChunks[chunkResourceID].resourceHeaders.Size();

    m_OldToNewIndices[chunkResourceID][m_SourceResourceRuntimeResourceID.GetIndexInLibrary()] = newIndexInLibrary;
    m_NewChunks[chunkResourceID].resourceHeaders.PushBack(*resourceHeader);
    m_ChunkResources[chunkResourceID].insert(m_SourceResourceRuntimeResourceID.GetID());

    int newChunkindex = 0;

    for (auto it = m_NewChunks.begin(); it != m_NewChunks.end(); ++it)
    {
        std::string oldChunkResourceID = it->first;
        std::string newChunkResourceID = GenerateHeaderLibraryChunkResourceID(resourceID, newChunkindex++);

        m_OldChunkResourceIDsToNewResourceIDs[oldChunkResourceID] = newChunkResourceID;
    }

    UpdateReferenceRuntimeResourceIDs();
    UpdateHeaderLibraryChunksData();
    CreateResourceLibraries();
    CreateHeaderLibrary(resourceID, p_NewHeaderLibraryResourceID);

    operator delete(m_HeaderLibrary, std::align_val_t(binaryDeserializer.GetAlignment()));
}

void DynamicHeaderLibraryGenerator::AddReferencesToChunks(TArray<unsigned char>* p_ResourceHeader, const ZRuntimeResourceID& p_RuntimeResourceID)
{
    SResourceHeaderHeader resourceHeaderHeader;
    unsigned char* referencesChunk = nullptr;

    memcpy(&resourceHeaderHeader, p_ResourceHeader->GetStart(), sizeof(SResourceHeaderHeader));

    if (resourceHeaderHeader.m_nReferencesChunkSize > 0)
    {
        referencesChunk = p_ResourceHeader->GetStart() + sizeof(SResourceHeaderHeader);
    }

    ZResourceHeaderReader resourceHeaderReader = ZResourceHeaderReader(resourceHeaderHeader, referencesChunk);

    for (uint32_t i = 0; i < resourceHeaderReader.GetNumResourceIdentifiers(); ++i)
    {
        ZRuntimeResourceID runtimeResourceID2 = resourceHeaderReader.GetResourceIdentifier(i);

        // Skip TEMP (some TEMP resource ids have both pc_entitytype and pc_entitytemplate)
        if (runtimeResourceID2 == p_RuntimeResourceID || runtimeResourceID2 == m_SourceResourceRuntimeResourceID)
        {
            continue;
        }

        if (!runtimeResourceID2.IsLibraryResource())
        {
            EResourceReferenceFlags resourceReferenceFlags = resourceHeaderReader.GetResourceFlags(i);

            if ((resourceReferenceFlags & EResourceReferenceFlags::RESOURCE_REFERENCE_INSTALL_DEPENDENCY)
                == EResourceReferenceFlags::RESOURCE_REFERENCE_INSTALL_DEPENDENCY)
            {
                m_GlobalResourceIDs.insert(SDK().GetResourceID(runtimeResourceID2));
            }

            continue;
        }

        auto it = m_ChunkHashesToIndices.find(runtimeResourceID2.m_IDLow);

        if (it != m_ChunkHashesToIndices.end())
        {
            const uint32_t chunkIndex = it->second;
            const uint32_t indexInLibrary = runtimeResourceID2.GetIndexInLibrary();
            SHeaderLibraryChunk* headerLibraryChunk = &m_HeaderLibrary->chunks[chunkIndex];

            if (indexInLibrary >= headerLibraryChunk->resourceHeaders.Size())
            {
                Logger::Error(
                    "Referenced resource index {} is out of range ({} resource headers).", indexInLibrary, headerLibraryChunk->resourceHeaders.Size()
                );

                continue;
            }

            const std::string resourceID = headerLibraryChunk->sLibraryID.ToCString();
            TArray<unsigned char>* resourceHeader2 = &headerLibraryChunk->resourceHeaders[indexInLibrary];

            if (m_ChunkResources[resourceID].find(runtimeResourceID2.GetID()) == m_ChunkResources[resourceID].end())
            {
                AddReferencesToChunks(resourceHeader2, runtimeResourceID2); // Resources that are referenced by resources must be above it

                const uint32_t newIndexInLibrary = m_NewChunks[resourceID].resourceHeaders.Size();

                m_OldToNewIndices[resourceID][runtimeResourceID2.GetIndexInLibrary()] = newIndexInLibrary;
                m_NewChunks[resourceID].resourceHeaders.PushBack(*resourceHeader2);
                m_ChunkResources[resourceID].insert(runtimeResourceID2.GetID());
            }
        }
    }
}

void DynamicHeaderLibraryGenerator::UpdateReferenceRuntimeResourceIDs()
{
    int index = 0;

    for (auto it = m_NewChunks.begin(); it != m_NewChunks.end(); ++it)
    {
        for (uint32_t i = 0; i < it->second.resourceHeaders.Size(); ++i)
        {
            TArray<unsigned char>* resourceHeader = &it->second.resourceHeaders[i];
            SResourceHeaderHeader resourceHeaderHeader;
            unsigned char* referencesChunk = nullptr;

            memcpy(&resourceHeaderHeader, resourceHeader->GetStart(), sizeof(SResourceHeaderHeader));

            if (resourceHeaderHeader.m_nReferencesChunkSize > 0)
            {
                referencesChunk = resourceHeader->GetStart() + sizeof(SResourceHeaderHeader);
            }

            ZResourceHeaderReader resourceHeaderReader = ZResourceHeaderReader(resourceHeaderHeader, referencesChunk);

            for (uint32_t j = 0; j < resourceHeaderReader.GetNumResourceIdentifiers(); ++j)
            {
                ZRuntimeResourceID runtimeResourceID = resourceHeaderReader.GetResourceIdentifier(j);

                if (m_ChunkHashesToResourceIDs.find(runtimeResourceID.m_IDLow) != m_ChunkHashesToResourceIDs.end())
                {
                    std::string chunkResourceID = m_ChunkHashesToResourceIDs[runtimeResourceID.m_IDLow];
                    std::map<uint32_t, uint32_t> oldToNewIndices2 = m_OldToNewIndices[chunkResourceID];
                    int indexInLibrary = runtimeResourceID.GetIndexInLibrary();

                    if (runtimeResourceID.IsLibraryResource() && oldToNewIndices2.find(indexInLibrary) != oldToNewIndices2.end())
                    {
                        std::string newChunkResourceID = m_OldChunkResourceIDsToNewResourceIDs[chunkResourceID];
                        uint64_t newChunkHash = hash::GetMD5Hash64(util::ToLowerCase(newChunkResourceID));
                        ZRuntimeResourceID newChunkRuntimeResourceID = ZRuntimeResourceID::Create(newChunkHash);

                        int newIndexInLibrary = oldToNewIndices2[runtimeResourceID.GetIndexInLibrary()];
                        uint32_t runtimeResourceIDOffset = 4 + j * 8;

                        runtimeResourceID = ZRuntimeResourceID::CreateLibraryResourceID(newChunkRuntimeResourceID, newIndexInLibrary);

                        uint64_t runtimeResourceID2 = runtimeResourceID.GetID();

                        if (referencesChunk)
                        {
                            memcpy(referencesChunk + runtimeResourceIDOffset, &runtimeResourceID2, sizeof(uint64_t));
                        }
                    }
                }
            }
        }
    }
}

std::string DynamicHeaderLibraryGenerator::GenerateHeaderLibraryResourceID(const std::string& p_ResourceID)
{
    return p_ResourceID.substr(0, p_ResourceID.find("]") + 1) + ".pc_headerlib";
}

std::string DynamicHeaderLibraryGenerator::GenerateHeaderLibraryChunkResourceID(const std::string& p_ResourceID, uint32_t p_Index)
{
    std::string indexString = std::to_string(p_Index);

    PadLeft(indexString, 4);

    return p_ResourceID.substr(0, p_ResourceID.find("]") + 1) + ".pc_resourcelibdef](" + indexString + ").pc_resourcelib";
}

void DynamicHeaderLibraryGenerator::PadLeft(std::string& p_String, size_t p_TargetLength, char p_PaddingChar)
{
    if (p_TargetLength > p_String.size())
    {
        p_String.insert(0, p_TargetLength - p_String.size(), p_PaddingChar);
    }
}

void DynamicHeaderLibraryGenerator::UpdateHeaderLibraryChunksData()
{
    for (auto it = m_NewChunks.begin(); it != m_NewChunks.end(); ++it)
    {
        hash::MD5Hash md5Hash = hash::MD5(util::ToLowerCase(it->first));
        uint32_t hash = std::strtoul(hash::MD5ToString(md5Hash).substr(8, 8).c_str(), nullptr, 16);
        uint32_t chunkIndex = m_ChunkHashesToIndices[hash];

        const SHeaderLibraryChunk* oldHeaderLibraryChunk = &m_HeaderLibrary->chunks[chunkIndex];
        SHeaderLibraryChunk* newHeaderLibraryChunk = &it->second;

        newHeaderLibraryChunk->nOffset = oldHeaderLibraryChunk->nOffset;

        if (oldHeaderLibraryChunk->nFlags == 0)
        {
            newHeaderLibraryChunk->nFlags = 2 | 16;
        }
        else
        {
            newHeaderLibraryChunk->nFlags = oldHeaderLibraryChunk->nFlags | 16;
            // newHeaderLibraryChunk->nFlags = 18;
            // newHeaderLibraryChunk->nFlags = oldHeaderLibraryChunk->nFlags;
        }

        newHeaderLibraryChunk->localizationCategory = oldHeaderLibraryChunk->localizationCategory;
        newHeaderLibraryChunk->sLanguages = oldHeaderLibraryChunk->sLanguages;
        newHeaderLibraryChunk->states = oldHeaderLibraryChunk->states;

        for (uint32_t i = 0; i < newHeaderLibraryChunk->resourceHeaders.Size(); ++i)
        {
            newHeaderLibraryChunk->ridMappingIndices.PushBack(i);
        }

        std::string oldChunkResourceID = it->first;
        std::string newChunkResourceID = m_OldChunkResourceIDsToNewResourceIDs[oldChunkResourceID];

        uint64_t newChunkHash = hash::GetMD5Hash64(util::ToLowerCase(newChunkResourceID));
        ZRuntimeResourceID newChunkRuntimeResourceID = ZRuntimeResourceID::Create(newChunkHash);
        int newIndexInLibrary = 0;

        it->second.sLibraryID.Allocate(newChunkResourceID.c_str(), newChunkResourceID.length());

        if (hash == m_SourceResourceRuntimeResourceID.m_IDLow)
        {
            m_SourceResourceRuntimeResourceID =
                ZRuntimeResourceID::CreateLibraryResourceID(newChunkRuntimeResourceID, newHeaderLibraryChunk->resourceHeaders.Size() - 1);
            newHeaderLibraryChunk->nFlags = 8;
            // newHeaderLibraryChunk->nFlags = 2;
        }

        for (auto it2 = m_ChunkResources[it->first].begin(); it2 != m_ChunkResources[it->first].end(); ++it2)
        {
            ZRuntimeResourceID newRuntimeResourceID = ZRuntimeResourceID::CreateLibraryResourceID(newChunkRuntimeResourceID, newIndexInLibrary++);

            newHeaderLibraryChunk->ridMappingIDs.PushBack(newRuntimeResourceID.GetID());
        }
    }
}

void DynamicHeaderLibraryGenerator::CreateHeaderLibrary(const std::string& p_ResourceID, std::string& p_NewHeaderLibraryResourceID)
{
    SHeaderLibrary* newHeaderLibrary = static_cast<SHeaderLibrary*>(operator new(sizeof(SHeaderLibrary), std::align_val_t(alignof(SHeaderLibrary))));

    memset(newHeaderLibrary, 0, sizeof(SHeaderLibrary));

    p_NewHeaderLibraryResourceID = GenerateHeaderLibraryResourceID(p_ResourceID);

    for (auto it = m_NewChunks.begin(); it != m_NewChunks.end(); ++it)
    {
        newHeaderLibrary->chunks.PushBack(it->second);
    }

    newHeaderLibrary->externalResourceIds = m_HeaderLibrary->externalResourceIds;
    newHeaderLibrary->ridSource = m_SourceResourceRuntimeResourceID.GetID();

    std::string newHeaderLibraryFilePath = util::ConvertResourceIDToFilePath(p_NewHeaderLibraryResourceID);

    ZBinarySerializer binarySerializer;
    void* resourceData = nullptr;
    uint32_t resourceDataSize = 0;

    binarySerializer.SerializeToMemory(newHeaderLibrary, &resourceData, resourceDataSize);

    BinaryWriter headerLibraryBinaryWriter = BinaryWriter(newHeaderLibraryFilePath);
    SResourceHeaderHeader resourceHeaderHeader;

    resourceHeaderHeader.m_type = 'HLIB';
    resourceHeaderHeader.m_nReferencesChunkSize = 0;
    resourceHeaderHeader.m_nStatesChunkSize = 0;
    resourceHeaderHeader.m_nDataSize = resourceDataSize;
    resourceHeaderHeader.m_nSystemMemoryRequirement = -1;
    resourceHeaderHeader.m_nVideoMemoryRequirement = -1;

    headerLibraryBinaryWriter.Write(&resourceHeaderHeader, sizeof SResourceHeaderHeader);

    uint32_t referencesChunkSize = 0;

    if (m_GlobalResourceIDs.size() > 0)
    {
        referencesChunkSize = 0x4 + 0x4 + 0x4 + 0x4 + m_GlobalResourceIDs.size() * 4;

        headerLibraryBinaryWriter.Write<uint32_t>(m_GlobalResourceIDs.size());
        headerLibraryBinaryWriter.Write<uint32_t>(0xC);
        headerLibraryBinaryWriter.Write<uint32_t>(headerLibraryBinaryWriter.GetPosition() + 0x8 + m_GlobalResourceIDs.size() * 0x4 - 0x18);
        headerLibraryBinaryWriter.Write<uint32_t>(EResourceReferenceFlags::RESOURCE_REFERENCE_INSTALL_DEPENDENCY);

        std::string referenceResourceID = *m_GlobalResourceIDs.begin();
        uint32_t resourceIDOffset = (referenceResourceID.length() + 1) & 0xFFFFFF | 0x80000000;

        headerLibraryBinaryWriter.Write<uint32_t>(resourceIDOffset);

        auto it = std::next(m_GlobalResourceIDs.begin(), 1);

        for (; it != m_GlobalResourceIDs.end(); ++it)
        {
            referenceResourceID = *it;
            resourceIDOffset += referenceResourceID.length() + 1;

            headerLibraryBinaryWriter.Write<uint32_t>(resourceIDOffset);
        }

        for (it = m_GlobalResourceIDs.begin(); it != m_GlobalResourceIDs.end(); ++it)
        {
            referenceResourceID = *it;
            referencesChunkSize += referenceResourceID.length() + 1;

            headerLibraryBinaryWriter.WriteString(referenceResourceID);
        }
    }

    headerLibraryBinaryWriter.Write(resourceData, resourceDataSize);

    if (m_GlobalResourceIDs.size() > 0)
    {
        headerLibraryBinaryWriter.Seek(4, SeekOrigin::Begin);
        headerLibraryBinaryWriter.Write<uint32_t>(referencesChunkSize);
    }

    operator delete(newHeaderLibrary, std::align_val_t(alignof(SHeaderLibrary)));

    if (m_FolderAlreadyExists)
    {
        m_GeneratedHeaderAndResourceLibraryFilePaths.push_back(newHeaderLibraryFilePath);
    }
}

void DynamicHeaderLibraryGenerator::CreateResourceLibrary(
    const std::string& p_OldChunkResourceID, const std::string& p_NewChunkResourceID,
    std::map<uint32_t, std::map<uint32_t, uint32_t>>& p_OldIndicesToOldOffsetsAndSizes, const std::map<uint32_t, uint32_t>& p_NewIndicesToOldIndices
)
{
    std::string oldResourceLibraryFilePath = util::ConvertResourceIDToFilePath(p_OldChunkResourceID);
    std::string newResourceLibraryFilePath = util::ConvertResourceIDToFilePath(p_NewChunkResourceID);
    std::string newResourceLibraryFolderPath = newResourceLibraryFilePath.substr(0, newResourceLibraryFilePath.find_last_of("/"));

    if (std::filesystem::exists(newResourceLibraryFolderPath))
    {
        m_FolderAlreadyExists = true;
    }
    else
    {
        std::filesystem::create_directories(newResourceLibraryFolderPath);
    }

    std::ifstream ifstream = std::ifstream(oldResourceLibraryFilePath, std::ios::binary);
    std::ofstream ofstream = std::ofstream(newResourceLibraryFilePath, std::ios::binary);
    std::vector<unsigned char> oldResourceLibraryData;

    oldResourceLibraryData.resize(24);

    ifstream.read(reinterpret_cast<char*>(oldResourceLibraryData.data()), 24);

    for (auto it = p_NewIndicesToOldIndices.begin(); it != p_NewIndicesToOldIndices.end(); ++it)
    {
        std::map<uint32_t, uint32_t> oldResourceDataOffsetAndSize = p_OldIndicesToOldOffsetsAndSizes[it->second];
        std::vector<unsigned char> resourceData;

        ifstream.seekg(oldResourceDataOffsetAndSize.begin()->first);

        resourceData.resize(oldResourceDataOffsetAndSize.begin()->second);
        ifstream.read(reinterpret_cast<char*>(resourceData.data()), oldResourceDataOffsetAndSize.begin()->second);
        ofstream.write(reinterpret_cast<char*>(resourceData.data()), resourceData.size());
    }

    ifstream.close();
    ofstream.close();

    std::ifstream ifstream2 = std::ifstream(newResourceLibraryFilePath, std::ios::binary | std::ios::ate);
    size_t position = static_cast<size_t>(ifstream2.tellg());
    std::vector<unsigned char> newResourceLibraryData;

    newResourceLibraryData.resize(position);
    ifstream2.seekg(0);
    ifstream2.read(reinterpret_cast<char*>(newResourceLibraryData.data()), position);
    ifstream2.close();

    uint32_t newDataSize = newResourceLibraryData.size();

    memcpy(oldResourceLibraryData.data() + 12, &newDataSize, sizeof(uint32_t));

    newResourceLibraryData.insert(newResourceLibraryData.begin(), oldResourceLibraryData.begin(), oldResourceLibraryData.end());

    std::ofstream ofstream2 = std::ofstream(newResourceLibraryFilePath, std::ios::binary | std::ios::ate);

    ofstream2.write(reinterpret_cast<char*>(newResourceLibraryData.data()), newResourceLibraryData.size());
    ofstream2.close();

    if (m_FolderAlreadyExists)
    {
        m_GeneratedHeaderAndResourceLibraryFilePaths.push_back(newResourceLibraryFilePath);
    }
    else if (m_NewFolderPath.empty())
    {
        m_NewFolderPath = newResourceLibraryFolderPath;
    }
}

void DynamicHeaderLibraryGenerator::CreateResourceLibraries()
{
    for (auto it = m_NewChunks.begin(); it != m_NewChunks.end(); ++it)
    {
        std::string oldChunkResourceID = it->first;
        std::string newChunkResourceID = m_OldChunkResourceIDsToNewResourceIDs[oldChunkResourceID];
        std::map<uint32_t, uint32_t> oldToNewIndices2 = m_OldToNewIndices[oldChunkResourceID];
        std::map<uint32_t, std::map<uint32_t, uint32_t>> oldIndicesToOldOffsetsAndSizes;
        std::map<uint32_t, uint32_t> newIndicesToOldIndices;

        for (uint32_t i = 0; i < m_HeaderLibrary->chunks.Size(); ++i)
        {
            std::string resourceID2 = m_HeaderLibrary->chunks[i].sLibraryID.ToCString();

            if (resourceID2 == oldChunkResourceID)
            {
                SHeaderLibraryChunk* headerLibraryChunk = &m_HeaderLibrary->chunks[i];
                uint32_t resourceOffset = 24;

                for (uint32_t j = 0; j < headerLibraryChunk->resourceHeaders.Size(); ++j)
                {
                    TArray<unsigned char>* resourceHeader = &headerLibraryChunk->resourceHeaders[j];
                    SResourceHeaderHeader resourceHeaderHeader;

                    memcpy(&resourceHeaderHeader, resourceHeader->GetStart(), sizeof(SResourceHeaderHeader));

                    if (resourceHeaderHeader.m_type == 'FSBM' || resourceHeaderHeader.m_type == 'FSBS')
                    {
                        resourceHeaderHeader.m_nDataSize += 24;
                    }

                    auto it2 = oldToNewIndices2.find(j);

                    if (it2 != oldToNewIndices2.end())
                    {
                        oldIndicesToOldOffsetsAndSizes[j].insert(std::make_pair(resourceOffset, resourceHeaderHeader.m_nDataSize));

                        if (oldIndicesToOldOffsetsAndSizes.size() == oldToNewIndices2.size())
                        {
                            break;
                        }
                    }

                    resourceOffset += resourceHeaderHeader.m_nDataSize;
                }

                break;
            }
        }

        for (auto it2 = oldToNewIndices2.begin(); it2 != oldToNewIndices2.end(); ++it2)
        {
            newIndicesToOldIndices.insert(std::make_pair(it2->second, it2->first));
        }

        CreateResourceLibrary(oldChunkResourceID, newChunkResourceID, oldIndicesToOldOffsetsAndSizes, newIndicesToOldIndices);
    }
}
