#include <Glacier/Resource/ZResourceLibraryLoader.h>

#include <Function.h>

ZResourcePending& ZResourceLibraryLoader::GetLibraryResource()
{
	return m_LibraryResource;
}

const ZResourceLibraryInfo* ZResourceLibraryLoader::GetLibraryEntries() const
{
	return m_pLibraryEntries;
}

ZResourceLibraryInfo* ZResourceLibraryLoader::GetLibraryEntries()
{
	return m_pLibraryEntries;
}

ZMutex* ZResourceLibraryLoader::GetBlocksMutex()
{
	return &m_BlocksMutex;
}

const unsigned int ZResourceLibraryLoader::GetEntryBytesLeft() const
{
	return m_nEntryBytesLeft;
}

const unsigned int ZResourceLibraryLoader::GetEntryIndex() const
{
	return m_nEntryIndex;
}

const TSharedPointer<ZResourceDataBuffer>& ZResourceLibraryLoader::GetCurrentData() const
{
	return m_pCurrentData;
}

TSharedPointer<ZResourceDataBuffer>& ZResourceLibraryLoader::GetFixedBuffer()
{
	return m_pFixedBuffer;
}

TSharedPointer<ZResourceDataBuffer>& ZResourceLibraryLoader::GetCurrentData()
{
	return m_pCurrentData;
}

void ZResourceLibraryLoader::SetFinished(const bool finished)
{
	m_bFinished = finished;
}

void ZResourceLibraryLoader::SetFailed(const bool failed)
{
	m_bFailed = failed;
}

void ZResourceLibraryLoader::SetLibraryEntries(ZResourceLibraryInfo* libraryEntries)
{
	m_pLibraryEntries = libraryEntries;
}

void ZResourceLibraryLoader::SetEntryBytesLeft(const unsigned int entryBytesLeft)
{
	m_nEntryBytesLeft = entryBytesLeft;
}

void ZResourceLibraryLoader::SetEntryIndex(const unsigned int entryIndex)
{
	m_nEntryIndex = entryIndex;
}

void ZResourceLibraryLoader::AllocateEntry(IResourceInstaller* pInstaller, unsigned int nSize, ZRuntimeResourceID ridResource)
{
	Function::CallMethod<ZResourceLibraryLoader*, IResourceInstaller*, unsigned int, ZRuntimeResourceID>(BaseAddress + 0x156280, this, pInstaller, nSize, ridResource);
}
