#pragma once

#include "ZResourcePending.h"
#include "ZResourceLibraryInfo.h"
#include "../ZMutex.h"
#include "ZResourceDataBuffer.h"

class ZResourceLibraryLoader
{
public:
	ZResourcePending& GetLibraryResource();
	const ZResourceLibraryInfo* GetLibraryEntries() const;
	ZResourceLibraryInfo* GetLibraryEntries();
	ZMutex* GetBlocksMutex();
	const unsigned int GetEntryBytesLeft() const;
	const unsigned int GetEntryIndex() const;
	const TSharedPointer<ZResourceDataBuffer>& GetCurrentData() const;
	TSharedPointer<ZResourceDataBuffer>& GetFixedBuffer();
	TSharedPointer<ZResourceDataBuffer>& GetCurrentData();
	void SetFinished(const bool finished);
	void SetFailed(const bool failed);
	void SetLibraryEntries(ZResourceLibraryInfo* libraryEntries);
	void SetEntryBytesLeft(const unsigned int entryBytesLeft);
	void SetEntryIndex(const unsigned int entryIndex);
	void AllocateEntry(IResourceInstaller* pInstaller, unsigned int nSize, ZRuntimeResourceID ridResource);

private:
	bool m_bFinished;
	bool m_bFailed;
	ZResourcePending m_LibraryResource;
	ZResourceLibraryInfo* m_pLibraryEntries;
	ZMutex m_BlocksMutex;
	unsigned int m_nMaxResourceSize;
	unsigned int m_nEntryBytesLeft;
	unsigned int m_nEntryIndex;
	TSharedPointer<ZResourceDataBuffer> m_pFixedBuffer;
	TSharedPointer<ZResourceDataBuffer> m_pCurrentData;
	unsigned int m_nReadPosition;
	unsigned int m_nParsePosition;
	unsigned int m_nLibraryFileSize;
	unsigned int m_nAllocationCount;
};
