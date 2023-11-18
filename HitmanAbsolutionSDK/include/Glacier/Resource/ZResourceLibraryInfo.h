#pragma once

#include "../Templates/TArray.h"
#include "ZResourceID.h"
#include "SResourceLibraryEntry.h"
#include "../Templates/TSharedPointer.h"
#include "ZResourceDataBuffer.h"

class HitmanAbsolutionSDK_API ZResourceLibraryInfo
{
public:
	ZResourceStub* GetLibraryStub() const;
	ZRuntimeResourceID GetSourceRuntimeResourceID() const;
	ZResourceID GetResourceID() const;
	const SResourceLibraryEntry& GetEntry(const unsigned int index) const;
	SResourceLibraryEntry& GetEntry(const unsigned int index);
	unsigned int GetEntryCount() const;
	bool IsValid(unsigned int nIndex) const;
	bool InstallResource(unsigned int nIndex, TSharedPointer<ZResourceDataBuffer> pResourceData);

private:
	enum
	{
		GLOBAL_LIBRARIES_STATE = -1,
		NON_STREAMED_LIBRARIES_STATE = -2,
		INIT_LIBRARIES_STATE = -3
	};

	enum
	{
		GLOBAL = 1,
		NON_STREAMED = 2,
		MEDIA_STREAMED = 4,
		INIT_LIBRARY = 8,
		DYNAMIC = 16
	};

	TArray<int> m_StreamingStates;
	ZResourceStub* m_pLibraryStub;
	ZResourceID m_ResourceID;
	ZRuntimeResourceID m_MapResourceID;
	ZRuntimeResourceID m_SourceResourceID;
	TArray<SResourceLibraryEntry> m_Entries;
	char m_Flags;
	unsigned int m_nStartFilePosition;
	TArray<unsigned int> m_EntryFilePositions;
	TArray<ZString> m_Languages;
	int m_LocalizationCategory;
};
