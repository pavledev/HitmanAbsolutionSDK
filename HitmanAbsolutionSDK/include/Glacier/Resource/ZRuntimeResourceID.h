#pragma once

#include <Common.h>

#include <string>

class HitmanAbsolutionSDK_API ZRuntimeResourceID
{
public:
	ZRuntimeResourceID();
	ZRuntimeResourceID(unsigned long long runtimeResourceID);
	ZRuntimeResourceID(unsigned int idHigh, unsigned int idLow);
	operator long long() const;
	unsigned long long GetID() const;
	bool operator==(const ZRuntimeResourceID& other) const;
	bool operator!=(const ZRuntimeResourceID& other) const;
	static std::string QueryResourceID(const ZRuntimeResourceID& ridResource);
	static ZRuntimeResourceID Create(unsigned long long nResource);
	static ZRuntimeResourceID CreateLibraryResourceID(ZRuntimeResourceID ridLibrary, int indexInLibrary);
	bool IsLibraryResource() const;
	bool IsLibrary() const;
	ZRuntimeResourceID GetLibraryRuntimeResourceID() const;
	int GetIndexInLibrary() const;

private:
	unsigned int m_IDHigh;
	unsigned int m_IDLow;
};
