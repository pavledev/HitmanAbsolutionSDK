#pragma once

#include "Templates/TArray.h"
#include "ZString.h"

class ZStringBuilder
{
public:
	ZStringBuilder();
	ZStringBuilder(const char* pszInitial, int nLength);
	ZStringBuilder(const ZString& sInitial);
	void Initialize(const char* pszData, int nLength);
	ZString ToString() const;

private:
	TArray<char> m_chars;
};
