#pragma once

#include "../ZString.h"

class HitmanAbsolutionSDK_API ZResourceID
{
public:
	ZResourceID(const ZString& rhs);
	ZResourceID(const char* rhs);
	const ZString& GetURI() const;
	bool IsLibraryResource() const;

private:
	ZString m_uri;
};
