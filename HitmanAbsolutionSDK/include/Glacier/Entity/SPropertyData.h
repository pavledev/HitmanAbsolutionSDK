#pragma once

#include "SPropertyInfo.h"

struct SPropertyData
{
	unsigned int m_nPropertyID;
	int m_nPropertyOffset;
	const SPropertyInfo* m_pInfo;
};
