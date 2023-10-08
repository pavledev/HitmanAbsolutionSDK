#pragma once

#include "../Templates/TArray.h"
#include "ZResourceID.h"
#include "SResourceLibraryEntry.h"

class ZResourceLibraryInfo
{
public:
	ZRuntimeResourceID GetSourceRuntimeResourceID() const;

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
