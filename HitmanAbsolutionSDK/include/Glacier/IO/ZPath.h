#pragma once

#include "../ZStringBuilder.h"

class ZPath
{
public:
	ZPath(const ZString& rhs);
	ZString ToString() const;

private:
	ZStringBuilder m_value;
};
