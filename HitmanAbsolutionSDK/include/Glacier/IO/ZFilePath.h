#pragma once

#include "ZPath.h"

class ZFilePath
{
public:
	ZFilePath(const ZString& rhs);
	ZString ToString() const;

private:
	ZPath m_path;
};
