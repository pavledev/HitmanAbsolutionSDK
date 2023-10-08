#pragma once

#include <Common.h>

struct HitmanAbsolutionSDK_API STokenID
{
	bool operator==(const STokenID& a) const;
	bool operator!=(const STokenID& a) const;

	unsigned int m_iValue;
	bool m_bValid;

	static STokenID InvalidToken;
};
