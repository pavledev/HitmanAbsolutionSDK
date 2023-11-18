#include <Glacier/Resource/ZResourceLibraryInfo.h>

#include <Function.h>

ZResourceStub* ZResourceLibraryInfo::GetLibraryStub() const
{
	return m_pLibraryStub;
}

ZRuntimeResourceID ZResourceLibraryInfo::GetSourceRuntimeResourceID() const
{
	return m_SourceResourceID;
}

ZResourceID ZResourceLibraryInfo::GetResourceID() const
{
	return m_ResourceID;
}

const SResourceLibraryEntry& ZResourceLibraryInfo::GetEntry(const unsigned int index) const
{
	return m_Entries[index];
}

SResourceLibraryEntry& ZResourceLibraryInfo::GetEntry(const unsigned int index)
{
	return m_Entries[index];
}

unsigned int ZResourceLibraryInfo::GetEntryCount() const
{
	return m_Entries.Size();
}

bool ZResourceLibraryInfo::IsValid(unsigned int nIndex) const
{
	return m_Entries[nIndex].pStub != 0;
}

bool ZResourceLibraryInfo::InstallResource(unsigned int nIndex, TSharedPointer<ZResourceDataBuffer> pResourceData)
{
	return Function::CallMethodAndReturn<bool, ZResourceLibraryInfo*, unsigned int, TSharedPointer<ZResourceDataBuffer>>(BaseAddress + 0x31DFC0, this, nIndex, pResourceData);
}
