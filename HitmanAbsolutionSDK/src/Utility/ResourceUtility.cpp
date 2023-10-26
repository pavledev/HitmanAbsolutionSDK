#include <string>
#include <filesystem>
#include <format>

#include <Glacier/Resource/ZRuntimeResourceID.h>
#include <Glacier/Resource/ZResourceManager.h>

#include <Utility/ResourceUtility.h>
#include <Utility/StringUtility.h>
#include <Hash.h>
#include <Resource/DynamicHeaderLibraryGenerator.h>
#include <Logger.h>

void ResourceUtility::LoadResource(const unsigned long long resourceRuntimeResourceID, const unsigned long long headerLibraryruntimeResourceID, void*& resourceData, unsigned int& resourceDataSize)
{
	const std::string headerLibraryResourceID = ZRuntimeResourceID::QueryResourceID(headerLibraryruntimeResourceID).GetURI().ToCString();
	const std::string headerLibraryFilePath = ResourceUtility::ConvertResourceIDToFilePath(headerLibraryResourceID);
	BinaryReader headerLibraryBinaryReader = BinaryReader(headerLibraryFilePath);

	headerLibraryBinaryReader.Seek(4, SeekOrigin::Begin);

	const unsigned int referencesChunkSize = headerLibraryBinaryReader.Read<unsigned int>();

	headerLibraryBinaryReader.Seek(referencesChunkSize + 24 + 16, SeekOrigin::Begin);

	unsigned int chunksStartOffset = headerLibraryBinaryReader.GetPosition();
	chunksStartOffset += headerLibraryBinaryReader.Read<unsigned int>();

	headerLibraryBinaryReader.Seek(chunksStartOffset - 4);

	const unsigned int chunkCount = headerLibraryBinaryReader.Read<unsigned int>();

	for (unsigned int i = 0; i < chunkCount; ++i)
	{
		const unsigned int chunkOffset = chunksStartOffset + 0x50 * i; //0x50 is size of SHeaderLibraryChunk

		headerLibraryBinaryReader.Seek(chunkOffset + 0x44, SeekOrigin::Begin);

		unsigned int runtimeResourceIDsStartOffset = headerLibraryBinaryReader.GetPosition();
		runtimeResourceIDsStartOffset += headerLibraryBinaryReader.Read<unsigned int>();

		headerLibraryBinaryReader.Seek(runtimeResourceIDsStartOffset - 4);

		const unsigned int ridMappingIDsCount = headerLibraryBinaryReader.Read<unsigned int>();
		unsigned int indexInLibrary = -1;

		for (unsigned int j = 0; j < ridMappingIDsCount; ++j)
		{
			headerLibraryBinaryReader.Seek(runtimeResourceIDsStartOffset + j * 0x8, SeekOrigin::Begin);

			const unsigned long long runtimeResourceID = headerLibraryBinaryReader.Read<unsigned long long>();

			if (runtimeResourceID == resourceRuntimeResourceID)
			{
				indexInLibrary = j;

				break;
			}
		}

		headerLibraryBinaryReader.Seek(chunkOffset + 0x4, SeekOrigin::Begin);

		unsigned int headerLibraryChunkResourceIDOffset = headerLibraryBinaryReader.GetPosition();
		headerLibraryChunkResourceIDOffset += headerLibraryBinaryReader.Read<unsigned int>();

		headerLibraryBinaryReader.Seek(headerLibraryChunkResourceIDOffset - 4, SeekOrigin::Begin);

		const unsigned int resourceIDLength = headerLibraryBinaryReader.Read<unsigned int>();
		const std::string headerLibraryChunkResourceID = headerLibraryBinaryReader.ReadString(resourceIDLength - 1);
		const std::string resourceLibraryFilePath = ResourceUtility::ConvertResourceIDToFilePath(headerLibraryChunkResourceID);

		headerLibraryBinaryReader.Seek(chunkOffset + 0x2C, SeekOrigin::Begin);

		unsigned int resourceHeadersStartOffset = headerLibraryBinaryReader.GetPosition();
		resourceHeadersStartOffset += headerLibraryBinaryReader.Read<unsigned int>();

		LoadResourceData(headerLibraryBinaryReader, resourceHeadersStartOffset, indexInLibrary, resourceLibraryFilePath, resourceData, resourceDataSize);
	}
}

void ResourceUtility::LoadResourceData(BinaryReader& headerLibraryBinaryReader, const unsigned int resourceHeadersStartOffset, const unsigned int indexInLibrary, const std::string& resourceLibraryFilePath, void*& resourceData, unsigned int& resourceDataSize)
{
	headerLibraryBinaryReader.Seek(resourceHeadersStartOffset - 4, SeekOrigin::Begin);

	unsigned int resourceHeaderCount = headerLibraryBinaryReader.Read<unsigned int>();
	unsigned int resourceLibraryOffset = 0x18;
	unsigned int currentPosition = 0;

	for (unsigned int i = 0; i < resourceHeaderCount; ++i)
	{
		currentPosition = headerLibraryBinaryReader.GetPosition();
		unsigned int resourceHeaderOffset = currentPosition + headerLibraryBinaryReader.Read<unsigned int>();
		headerLibraryBinaryReader.Seek(resourceHeaderOffset, SeekOrigin::Begin);

		unsigned int type = headerLibraryBinaryReader.Read<unsigned int>();

		headerLibraryBinaryReader.Skip(8);

		unsigned int dataSize = headerLibraryBinaryReader.Read<unsigned int>();

		if (type == 'FSBM' || type == 'FSBS')
		{
			dataSize += 24;
		}

		if (i == indexInLibrary)
		{
			BinaryReader resourceLibraryBinaryReader = BinaryReader(resourceLibraryFilePath);

			resourceLibraryBinaryReader.Seek(resourceLibraryOffset);

			resourceDataSize = dataSize;
			resourceData = resourceLibraryBinaryReader.Read<void>(resourceDataSize);

			break;
		}

		resourceLibraryOffset += dataSize;

		headerLibraryBinaryReader.Seek(currentPosition + 0xC, SeekOrigin::Begin);
	}
}

std::string ResourceUtility::ConvertResourceIDToFilePath(const std::string& resourceID)
{
	std::string resourceID2 = StringUtility::ToLowerCase(resourceID);
	const std::string extension = resourceID2.substr(resourceID2.find_last_of('.') + 1);
	const int index = static_cast<int>(resourceID2.find(':'));
	const int index2 = static_cast<int>(resourceID2.find('?'));
	const int index3 = static_cast<int>(resourceID2.find(']'));
	std::string folderPath = std::format("{}/runtime", std::filesystem::current_path().string());

	if (index2 != -1 && index < index2)
	{
		folderPath += resourceID2.substr(index + 1, index2 - index - 1);
	}
	else
	{
		folderPath += resourceID2.substr(index + 1, index3 - index - 1);
	}

	resourceID2 = resourceID2.substr(0, resourceID2.find_last_of('.') + 1);

	const Hash::MD5Hash md5Hash = Hash::MD5(resourceID2);
	const std::string fileName = Hash::ConvertMD5ToString(md5Hash);

	return std::format("{}/{}.{}", folderPath, fileName, extension);
}

std::filesystem::path ResourceUtility::GetHeaderLibraryPath(const unsigned long long resourceRuntimeResourceID)
{
	for (auto& p : std::filesystem::recursive_directory_iterator("runtime"))
	{
		if (p.path().extension() != ".pc_headerlib")
		{
			continue;
		}

		BinaryReader headerLibraryBinaryReader = BinaryReader(p.path().string());

		headerLibraryBinaryReader.Seek(4, SeekOrigin::Begin);

		const unsigned int referencesChunkSize = headerLibraryBinaryReader.Read<unsigned int>();

		headerLibraryBinaryReader.Seek(referencesChunkSize + 24 + 16, SeekOrigin::Begin);

		unsigned int chunksStartOffset = headerLibraryBinaryReader.GetPosition();
		chunksStartOffset += headerLibraryBinaryReader.Read<unsigned int>();

		headerLibraryBinaryReader.Seek(chunksStartOffset - 4);

		const unsigned int chunkCount = headerLibraryBinaryReader.Read<unsigned int>();

		for (unsigned int i = 0; i < chunkCount; ++i)
		{
			const unsigned int chunkOffset = chunksStartOffset + 0x50 * i; //0x50 is size of SHeaderLibraryChunk

			headerLibraryBinaryReader.Seek(chunkOffset + 0x44, SeekOrigin::Begin);

			unsigned int runtimeResourceIDsStartOffset = headerLibraryBinaryReader.GetPosition();
			runtimeResourceIDsStartOffset += headerLibraryBinaryReader.Read<unsigned int>();

			headerLibraryBinaryReader.Seek(runtimeResourceIDsStartOffset - 4);

			const unsigned int ridMappingIDsCount = headerLibraryBinaryReader.Read<unsigned int>();
			unsigned int indexInLibrary = -1;

			for (unsigned int j = 0; j < ridMappingIDsCount; ++j)
			{
				headerLibraryBinaryReader.Seek(runtimeResourceIDsStartOffset + j * 0x8, SeekOrigin::Begin);

				const unsigned long long runtimeResourceID = headerLibraryBinaryReader.Read<unsigned long long>();

				if (runtimeResourceID == resourceRuntimeResourceID)
				{
					return p.path();
				}
			}
		}
	}

	return {};
}

bool ResourceUtility::CreateAndInstallDynamicResourceLibrary(ZDynamicResourceLibrary*& dynamicResourceLibrary, const std::string& resourceID, ZRuntimeResourceID& tempRuntimeResourceID, const unsigned int entityCount)
{
	ZRuntimeResourceID resourceRuntimeResourceID = ZRuntimeResourceID::QueryRuntimeResourceID(resourceID.c_str());
	std::filesystem::path headerLibraryFilePath = GetHeaderLibraryPath(resourceRuntimeResourceID);
	std::string newHeaderLibraryResourceID;
	DynamicHeaderLibraryGenerator dynamicHeaderLibraryGenerator;

	dynamicHeaderLibraryGenerator.ExtractResourceToHeaderLibrary(resourceID, headerLibraryFilePath.string(), newHeaderLibraryResourceID);

	return InstallDynamicResourceLibrary(dynamicResourceLibrary, newHeaderLibraryResourceID, tempRuntimeResourceID, entityCount);
}

bool ResourceUtility::InstallDynamicResourceLibrary(ZDynamicResourceLibrary*& dynamicResourceLibrary, const std::string& headerLibraryResourceID, ZRuntimeResourceID& tempRuntimeResourceID, const unsigned int entityCount)
{
	const ZRuntimeResourceID headerLibraryRuntimeResourceID = ZRuntimeResourceID::QueryRuntimeResourceID(headerLibraryResourceID.c_str());

	return InstallDynamicResourceLibrary(dynamicResourceLibrary, headerLibraryRuntimeResourceID, tempRuntimeResourceID, entityCount);
}

bool ResourceUtility::InstallDynamicResourceLibrary(ZDynamicResourceLibrary*& dynamicResourceLibrary, const ZRuntimeResourceID& headerLibraryRuntimeResourceID, ZRuntimeResourceID& tempRuntimeResourceID, const unsigned int entityCount)
{
	tempRuntimeResourceID = GetTEMPRuntimeResourceID(headerLibraryRuntimeResourceID);
	dynamicResourceLibrary = new ZDynamicResourceLibrary(headerLibraryRuntimeResourceID, false, entityCount);

	if (!dynamicResourceLibrary->IsReady())
	{
		dynamicResourceLibrary->WaitForResources();
	}

	if (dynamicResourceLibrary->IsFailed())
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed install dynamic resource library!");

		delete dynamicResourceLibrary;

		return false;
	}

	return true;
}

ZRuntimeResourceID ResourceUtility::GetTEMPRuntimeResourceID(const ZRuntimeResourceID& headerLibraryRuntimeResourceID)
{
	TResourcePtr<ZHeaderLibrary> headerLibraryResourcePtr = ResourceManager->LoadResource(headerLibraryRuntimeResourceID);
	const ZResourcePtr tempResourcePtr = headerLibraryResourcePtr->GetSourceResource();
	const ZRuntimeResourceID tempRuntimeResourceID = tempResourcePtr.GetResourceStub()->GetRuntimeResourceID();

	return tempRuntimeResourceID;
}
