#include <filesystem>

#include <Glacier/Serializer/ZBinaryDeserializer.h>
#include <Glacier/Serializer/ZBinarySerializer.h>
#include <Glacier/Resource/SResourceHeaderHeader.h>
#include <Glacier/Resource/ZResourceHeaderReader.h>

#include <Resource/DynamicHeaderLibraryGenerator.h>
#include <Registry/ResourceIDRegistry.h>
#include <Utility/ResourceUtility.h>
#include <Hash.h>
#include <Utility/StringUtility.h>

DynamicHeaderLibraryGenerator::~DynamicHeaderLibraryGenerator()
{
	if (folderAlreadyExists)
	{
		for (size_t i = 0; i < generatedHeaderAndResourceLibraryFilePaths.size(); ++i)
		{
			std::filesystem::remove(generatedHeaderAndResourceLibraryFilePaths[i]);
		}
	}
	else
	{
		std::filesystem::remove_all(newFolderPath);
	}
}

void DynamicHeaderLibraryGenerator::ExtractResourceToHeaderLibrary(const std::string& resourceID, const std::string& headerLibraryFilePath, std::string& newHeaderLibraryResourceID)
{
	std::string resourceID2;

	if (resourceID.contains("aspectdummy"))
	{
		const size_t index = resourceID.find("(");

		resourceID2 = resourceID.substr(index + 1, resourceID.find(",") - index - 1);
	}
	else
	{
		resourceID2 = resourceID;
	}

	if (resourceID2.contains(" "))
	{
		StringUtility::ReplaceAll(resourceID2, " ", "");
	}

	sourceResourceRuntimeResourceID = ZRuntimeResourceID::QueryRuntimeResourceID(resourceID.c_str());

	ZBinaryDeserializer binaryDeserializer;
	BinaryReader headerBinaryReader = BinaryReader(headerLibraryFilePath);

	headerBinaryReader.Seek(4, SeekOrigin::Begin);

	unsigned int referencesChunkSize = headerBinaryReader.Read<unsigned int>();

	headerBinaryReader.Seek(referencesChunkSize + 24, SeekOrigin::Begin);

	headerLibrary = static_cast<SHeaderLibrary*>(binaryDeserializer.Deserialize(headerBinaryReader));

	for (unsigned int i = 0; i < headerLibrary->chunks.Size(); ++i)
	{
		std::string resourceID2 = headerLibrary->chunks[i].sLibraryID.ToCString();
		std::string resourceLibraryFilePath = ResourceUtility::ConvertResourceIDToFilePath(resourceID2);

		if (!std::filesystem::exists(resourceLibraryFilePath))
		{
			continue;
		}

		Hash::MD5Hash md5Hash = Hash::MD5(StringUtility::ToLowerCase(resourceID2));
		unsigned int hash = std::strtoul(Hash::ConvertMD5ToString(md5Hash).substr(8, 8).c_str(), nullptr, 16);

		chunkHashesToIndices[hash] = i;
		chunkHashesToResourceIDs[hash] = resourceID2;
	}

	const unsigned int chunkIndex = chunkHashesToIndices[sourceResourceRuntimeResourceID.GetIDLow()];
	const unsigned int indexInLibrary = sourceResourceRuntimeResourceID.GetIndexInLibrary();
	SHeaderLibraryChunk* headerLibraryChunk = &headerLibrary->chunks[chunkIndex];
	const std::string chunkResourceID = headerLibraryChunk->sLibraryID.ToCString();
	TArray<unsigned char>* resourceHeader = &headerLibraryChunk->resourceHeaders[indexInLibrary];

	AddReferencesToChunks(resourceHeader, sourceResourceRuntimeResourceID);

	const unsigned int newIndexInLibrary = newChunks[chunkResourceID].resourceHeaders.Size();

	oldToNewIndices[chunkResourceID][sourceResourceRuntimeResourceID.GetIndexInLibrary()] = newIndexInLibrary;
	newChunks[chunkResourceID].resourceHeaders.PushBack(*resourceHeader);
	chunkResources[chunkResourceID].insert(sourceResourceRuntimeResourceID.GetID());

	int newChunkindex = 0;

	for (auto it = newChunks.begin(); it != newChunks.end(); ++it)
	{
		std::string oldChunkResourceID = it->first;
		std::string newChunkResourceID = GenerateHeaderLibraryChunkResourceID(resourceID2, newChunkindex++);

		oldChunkResourceIDsToNewResourceIDs[oldChunkResourceID] = newChunkResourceID;
	}

	UpdateReferenceRuntimeResourceIDs(resourceID2);
	UpdateHeaderLibraryChunksData(resourceID2);
	CreateResourceLibraries();
	CreateHeaderLibrary(resourceID2, headerLibraryFilePath, newHeaderLibraryResourceID);

	operator delete(headerLibrary, std::align_val_t(binaryDeserializer.GetAlignment()));
}

void DynamicHeaderLibraryGenerator::AddReferencesToChunks(TArray<unsigned char>* resourceHeader, const ZRuntimeResourceID& runtimeResourceID)
{
	SResourceHeaderHeader resourceHeaderHeader;
	unsigned char* referencesChunk = nullptr;

	memcpy(&resourceHeaderHeader, resourceHeader->GetStart(), sizeof(SResourceHeaderHeader));

	if (resourceHeaderHeader.m_nReferencesChunkSize > 0)
	{
		referencesChunk = resourceHeader->GetStart() + sizeof(SResourceHeaderHeader);
	}

	ZResourceHeaderReader resourceHeaderReader = ZResourceHeaderReader(resourceHeaderHeader, referencesChunk);

	for (unsigned int i = 0; i < resourceHeaderReader.GetNumResourceIdentifiers(); ++i)
	{
		ZRuntimeResourceID runtimeResourceID2 = resourceHeaderReader.GetResourceIdentifier(i);

		//Skip TEMP (some TEMP resource ids have both pc_entitytype and pc_entitytemplate)
		if (runtimeResourceID2 == runtimeResourceID || runtimeResourceID2 == sourceResourceRuntimeResourceID)
		{
			continue;
		}

		if (!runtimeResourceID2.IsLibraryResource())
		{
			EResourceReferenceFlags resourceReferenceFlags = resourceHeaderReader.GetResourceFlags(i);

			if ((resourceReferenceFlags & EResourceReferenceFlags::RESOURCE_REFERENCE_INSTALL_DEPENDENCY) == EResourceReferenceFlags::RESOURCE_REFERENCE_INSTALL_DEPENDENCY)
			{
				globalResourceIDs.insert(ZRuntimeResourceID::QueryResourceID(runtimeResourceID2).GetURI().ToCString());
			}

			continue;
		}

		auto it = chunkHashesToIndices.find(runtimeResourceID2.GetIDLow());

		if (it != chunkHashesToIndices.end())
		{
			const unsigned int chunkIndex = it->second;
			const unsigned int indexInLibrary = runtimeResourceID2.GetIndexInLibrary();
			SHeaderLibraryChunk* headerLibraryChunk = &headerLibrary->chunks[chunkIndex];
			const std::string resourceID = headerLibraryChunk->sLibraryID.ToCString();
			TArray<unsigned char>* resourceHeader2 = &headerLibraryChunk->resourceHeaders[indexInLibrary];

			if (chunkResources[resourceID].find(runtimeResourceID2.GetID()) == chunkResources[resourceID].end())
			{
				AddReferencesToChunks(resourceHeader2, runtimeResourceID2); //Resources that are referenced by resources must be above it

				const unsigned int newIndexInLibrary = newChunks[resourceID].resourceHeaders.Size();

				oldToNewIndices[resourceID][runtimeResourceID2.GetIndexInLibrary()] = newIndexInLibrary;
				newChunks[resourceID].resourceHeaders.PushBack(*resourceHeader2);
				chunkResources[resourceID].insert(runtimeResourceID2.GetID());
			}
		}
	}
}

void DynamicHeaderLibraryGenerator::UpdateReferenceRuntimeResourceIDs(const std::string& resourceID)
{
	int index = 0;

	for (auto it = newChunks.begin(); it != newChunks.end(); ++it)
	{
		for (unsigned int i = 0; i < it->second.resourceHeaders.Size(); ++i)
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

			for (unsigned int j = 0; j < resourceHeaderReader.GetNumResourceIdentifiers(); ++j)
			{
				ZRuntimeResourceID runtimeResourceID = resourceHeaderReader.GetResourceIdentifier(j);

				if (chunkHashesToResourceIDs.find(runtimeResourceID.GetIDLow()) != chunkHashesToResourceIDs.end())
				{
					std::string chunkResourceID = chunkHashesToResourceIDs[runtimeResourceID.GetIDLow()];
					std::map<unsigned int, unsigned int> oldToNewIndices2 = oldToNewIndices[chunkResourceID];
					int indexInLibrary = runtimeResourceID.GetIndexInLibrary();

					if (runtimeResourceID.IsLibraryResource() && oldToNewIndices2.find(indexInLibrary) != oldToNewIndices2.end())
					{
						std::string newChunkResourceID = oldChunkResourceIDsToNewResourceIDs[chunkResourceID];
						unsigned long long newChunkHash = Hash::GetMD5(StringUtility::ToLowerCase(newChunkResourceID));
						ZRuntimeResourceID newChunkRuntimeResourceID = ZRuntimeResourceID::Create(newChunkHash);

						int newIndexInLibrary = oldToNewIndices2[runtimeResourceID.GetIndexInLibrary()];
						unsigned int runtimeResourceIDOffset = 4 + j * 8;

						runtimeResourceID = ZRuntimeResourceID::CreateLibraryResourceID(newChunkRuntimeResourceID, newIndexInLibrary);

						unsigned long long runtimeResourceID2 = runtimeResourceID.GetID();

						if (referencesChunk)
						{
							memcpy(referencesChunk + runtimeResourceIDOffset, &runtimeResourceID2, sizeof(unsigned long long));
						}
					}
				}
			}
		}
	}
}

std::string DynamicHeaderLibraryGenerator::GenerateHeaderLibraryResourceID(const std::string& resourceID)
{
	return resourceID.substr(0, resourceID.find("]") + 1) + ".pc_headerlib";
}

std::string DynamicHeaderLibraryGenerator::GenerateHeaderLibraryChunkResourceID(const std::string& resourceID, const unsigned int index)
{
	std::string number = std::to_string(index);

	PadTo(number, 4);

	return resourceID.substr(0, resourceID.find("]") + 1) + ".pc_resourcelibdef](" + number + ").pc_resourcelib";
}

void DynamicHeaderLibraryGenerator::PadTo(std::string& str, const size_t num, const char paddingChar)
{
	if (num > str.size())
	{
		str.insert(0, num - str.size(), paddingChar);
	}
}

void DynamicHeaderLibraryGenerator::UpdateHeaderLibraryChunksData(const std::string& resourceID)
{
	for (auto it = newChunks.begin(); it != newChunks.end(); ++it)
	{
		Hash::MD5Hash md5Hash = Hash::MD5(StringUtility::ToLowerCase(it->first));
		unsigned int hash = std::strtoul(Hash::ConvertMD5ToString(md5Hash).substr(8, 8).c_str(), nullptr, 16);
		unsigned int chunkIndex = chunkHashesToIndices[hash];

		const SHeaderLibraryChunk* oldHeaderLibraryChunk = &headerLibrary->chunks[chunkIndex];
		SHeaderLibraryChunk* newHeaderLibraryChunk = &it->second;

		newHeaderLibraryChunk->nOffset = oldHeaderLibraryChunk->nOffset;

		if (oldHeaderLibraryChunk->nFlags == 0)
		{
			newHeaderLibraryChunk->nFlags = 2 | 16;
		}
		else
		{
			newHeaderLibraryChunk->nFlags = oldHeaderLibraryChunk->nFlags | 16;
			//newHeaderLibraryChunk->nFlags = 18;
			//newHeaderLibraryChunk->nFlags = oldHeaderLibraryChunk->nFlags;
		}

		newHeaderLibraryChunk->localizationCategory = oldHeaderLibraryChunk->localizationCategory;
		newHeaderLibraryChunk->sLanguages = oldHeaderLibraryChunk->sLanguages;
		//newHeaderLibraryChunk->states = oldHeaderLibraryChunk->states;

		for (unsigned int i = 0; i < newHeaderLibraryChunk->resourceHeaders.Size(); ++i)
		{
			newHeaderLibraryChunk->ridMappingIndices.PushBack(i);
		}

		std::string oldChunkResourceID = it->first;
		std::string newChunkResourceID = oldChunkResourceIDsToNewResourceIDs[oldChunkResourceID];

		unsigned long long newChunkHash = Hash::GetMD5(StringUtility::ToLowerCase(newChunkResourceID));
		ZRuntimeResourceID newChunkRuntimeResourceID = ZRuntimeResourceID::Create(newChunkHash);
		int newIndexInLibrary = 0;

		it->second.sLibraryID.Allocate(newChunkResourceID.c_str(), newChunkResourceID.length());

		if (hash == sourceResourceRuntimeResourceID.GetIDLow())
		{
			sourceResourceRuntimeResourceID = ZRuntimeResourceID::CreateLibraryResourceID(newChunkRuntimeResourceID, newHeaderLibraryChunk->resourceHeaders.Size() - 1);
			newHeaderLibraryChunk->nFlags = 8;
			//newHeaderLibraryChunk->nFlags = 2;
		}

		for (auto it2 = chunkResources[it->first].begin(); it2 != chunkResources[it->first].end(); ++it2)
		{
			ZRuntimeResourceID newRuntimeResourceID = ZRuntimeResourceID::CreateLibraryResourceID(newChunkRuntimeResourceID, newIndexInLibrary++);

			newHeaderLibraryChunk->ridMappingIDs.PushBack(newRuntimeResourceID.GetID());
		}
	}
}

void DynamicHeaderLibraryGenerator::CreateHeaderLibrary(const std::string& resourceID, const std::string& headerLibraryPath, std::string& newHeaderLibraryResourceID)
{
	SHeaderLibrary* newHeaderLibrary = static_cast<SHeaderLibrary*>(operator new(sizeof(SHeaderLibrary), std::align_val_t(alignof(SHeaderLibrary))));

	memset(newHeaderLibrary, 0, sizeof(SHeaderLibrary));

	newHeaderLibraryResourceID = GenerateHeaderLibraryResourceID(resourceID);

	for (auto it = newChunks.begin(); it != newChunks.end(); ++it)
	{
		newHeaderLibrary->chunks.PushBack(it->second);
	}

	newHeaderLibrary->externalResourceIds = headerLibrary->externalResourceIds;
	newHeaderLibrary->ridSource = sourceResourceRuntimeResourceID.GetID();

	std::string newHeaderLibraryFilePath = ResourceUtility::ConvertResourceIDToFilePath(newHeaderLibraryResourceID);

	ZBinarySerializer binarySerializer;
	void* resourceData = nullptr;
	unsigned int resourceDataSize = 0;

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

	unsigned int referencesChunkSize = 0;

	if (globalResourceIDs.size() > 0)
	{
		referencesChunkSize = 0x4 + 0x4 + 0x4 + 0x4 + globalResourceIDs.size() * 4;

		headerLibraryBinaryWriter.Write<unsigned int>(globalResourceIDs.size());
		headerLibraryBinaryWriter.Write<unsigned int>(0xC);
		headerLibraryBinaryWriter.Write<unsigned int>(headerLibraryBinaryWriter.GetPosition() + 0x8 + globalResourceIDs.size() * 0x4 - 0x18);
		headerLibraryBinaryWriter.Write<unsigned int>(EResourceReferenceFlags::RESOURCE_REFERENCE_INSTALL_DEPENDENCY);

		std::string referenceResourceID = *globalResourceIDs.begin();
		unsigned int resourceIDOffset = (referenceResourceID.length() + 1) & 0xFFFFFF | 0x80000000;

		headerLibraryBinaryWriter.Write<unsigned int>(resourceIDOffset);

		auto it = std::next(globalResourceIDs.begin(), 1);

		for (; it != globalResourceIDs.end(); ++it)
		{
			referenceResourceID = *it;
			resourceIDOffset += referenceResourceID.length() + 1;

			headerLibraryBinaryWriter.Write<unsigned int>(resourceIDOffset);
		}

		for (it = globalResourceIDs.begin(); it != globalResourceIDs.end(); ++it)
		{
			referenceResourceID = *it;
			referencesChunkSize += referenceResourceID.length() + 1;

			headerLibraryBinaryWriter.WriteString(referenceResourceID);
		}
	}

	headerLibraryBinaryWriter.Write(resourceData, resourceDataSize);

	if (globalResourceIDs.size() > 0)
	{
		headerLibraryBinaryWriter.Seek(4, SeekOrigin::Begin);
		headerLibraryBinaryWriter.Write<unsigned int>(referencesChunkSize);
	}

	operator delete(newHeaderLibrary, std::align_val_t(alignof(SHeaderLibrary)));

	if (folderAlreadyExists)
	{
		generatedHeaderAndResourceLibraryFilePaths.push_back(newHeaderLibraryFilePath);
	}
}

void DynamicHeaderLibraryGenerator::CreateResourceLibrary(const std::string& oldChunkResourceID, const std::string& newChunkResourceID,
	std::map<unsigned int, std::map<unsigned int, unsigned int>>& oldIndicesToOldOffsetsAndSizes, const std::map<unsigned int, unsigned int>& newIndicesToOldIndices)
{
	std::string oldResourceLibraryFilePath = ResourceUtility::ConvertResourceIDToFilePath(oldChunkResourceID);
	std::string newResourceLibraryFilePath = ResourceUtility::ConvertResourceIDToFilePath(newChunkResourceID);
	std::string newResourceLibraryFolderPath = newResourceLibraryFilePath.substr(0, newResourceLibraryFilePath.find_last_of("/"));

	if (std::filesystem::exists(newResourceLibraryFolderPath))
	{
		folderAlreadyExists = true;
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

	for (auto it = newIndicesToOldIndices.begin(); it != newIndicesToOldIndices.end(); ++it)
	{
		std::map<unsigned int, unsigned int> oldResourceDataOffsetAndSize = oldIndicesToOldOffsetsAndSizes[it->second];
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

	unsigned int newDataSize = newResourceLibraryData.size();

	memcpy(oldResourceLibraryData.data() + 12, &newDataSize, sizeof(unsigned int));

	newResourceLibraryData.insert(newResourceLibraryData.begin(), oldResourceLibraryData.begin(), oldResourceLibraryData.end());

	std::ofstream ofstream2 = std::ofstream(newResourceLibraryFilePath, std::ios::binary | std::ios::ate);

	ofstream2.write(reinterpret_cast<char*>(newResourceLibraryData.data()), newResourceLibraryData.size());
	ofstream2.close();

	if (folderAlreadyExists)
	{
		generatedHeaderAndResourceLibraryFilePaths.push_back(newResourceLibraryFilePath);
	}
	else if (newFolderPath.empty())
	{
		newFolderPath = newResourceLibraryFolderPath;
	}
}

void DynamicHeaderLibraryGenerator::CreateResourceLibraries()
{
	for (auto it = newChunks.begin(); it != newChunks.end(); ++it)
	{
		std::string oldChunkResourceID = it->first;
		std::string newChunkResourceID = oldChunkResourceIDsToNewResourceIDs[oldChunkResourceID];
		std::map<unsigned int, unsigned int> oldToNewIndices2 = oldToNewIndices[oldChunkResourceID];
		std::map<unsigned int, std::map<unsigned int, unsigned int>> oldIndicesToOldOffsetsAndSizes;
		std::map<unsigned int, unsigned int> newIndicesToOldIndices;

		for (unsigned int i = 0; i < headerLibrary->chunks.Size(); ++i)
		{
			std::string resourceID2 = headerLibrary->chunks[i].sLibraryID.ToCString();

			if (resourceID2 == oldChunkResourceID)
			{
				SHeaderLibraryChunk* headerLibraryChunk = &headerLibrary->chunks[i];
				unsigned int resourceOffset = 24;

				for (unsigned int j = 0; j < headerLibraryChunk->resourceHeaders.Size(); ++j)
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
