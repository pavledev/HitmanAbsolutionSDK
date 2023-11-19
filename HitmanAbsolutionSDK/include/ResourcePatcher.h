#pragma once

#include <unordered_map>
#include <filesystem>

#include <Glacier/Resource/ZRuntimeResourceID.h>

class ZHeaderLibraryInstaller;
class ZResourcePending;
class ZResourceLibraryInfo;
template <typename T> class TSharedPointer;
class ZResourceDataBuffer;
class ZResourceLibraryLoader;
class ZBufferBlock;
struct SResourceLibraryEntry;
class IResourceInstaller;

bool __fastcall ZHeaderLibraryInstaller_InstallHook(ZHeaderLibraryInstaller* pThis, int edx, ZResourcePending& ResourcePending);
bool __fastcall ZResourceLibraryLoader_ProcessBlockHook(ZResourceLibraryLoader* pThis, int edx, ZBufferBlock* pBlock);
void __fastcall ZResourceLibraryLoader_AllocateEntryHook(ZResourceLibraryLoader* pThis, int edx, IResourceInstaller* pInstaller, unsigned int nSize, ZRuntimeResourceID ridResource);
void __fastcall ZResourceLibraryLoader_StartLoadingHook(ZResourceLibraryLoader* pThis, int edx);

class ResourcePatcher
{
public:
	struct PatchedResource
	{
		unsigned long long runtimeResourceID;
		unsigned int resourceDataOffset;
		unsigned int resourceDataSize;
		std::string patchFileName;
	};

	ResourcePatcher();
	void LoadPatchedResources();
	const PatchedResource* GetPatchedResource(const ZRuntimeResourceID& runtimeResourceID) const;
	void GetPatchedResource(const ZRuntimeResourceID& runtimeResourceID, void*& resourceData, unsigned int& resourceDataSize);
	void SetCurrentHeaderLibraryRuntimeResourceID(const ZRuntimeResourceID& currentHeaderLibraryRuntimeResourceID);
	const bool IsResourcePatched(const ZRuntimeResourceID& runtimeResourceID) const;

	void OnHeaderLibraryInstall(ZHeaderLibraryInstaller* headerLibraryInstaller, ZResourcePending& resourcePending);
	bool OnProcessBlock(ZResourceLibraryLoader* resourceLibraryLoader, ZBufferBlock* block);
	void OnAllocateEntry(ZResourceLibraryLoader* resourceLibraryLoader, IResourceInstaller* resourceInstaller, const unsigned int size, const ZRuntimeResourceID& runtimeResourceID);
	void OnStartLoading(ZResourceLibraryLoader* resourceLibraryLoader);

private:
	std::unordered_map<unsigned long long, std::vector<PatchedResource>> patchedResources;
	ZRuntimeResourceID currentHeaderLibraryRuntimeResourceID;
};
