#pragma once

struct SPinInfo;

struct SPinData
{
	unsigned int m_nPinID;
	int m_nOffsetToThisPtr;
	const SPinInfo* m_pInfo;
};
