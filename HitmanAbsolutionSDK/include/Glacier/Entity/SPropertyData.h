#pragma once

struct SPropertyInfo;

struct SPropertyData
{
	unsigned int m_nPropertyID;
	int m_nPropertyOffset;
	const SPropertyInfo* m_pInfo;
};
