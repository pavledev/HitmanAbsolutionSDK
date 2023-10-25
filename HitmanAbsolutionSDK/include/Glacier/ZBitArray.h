#pragma once

#include "Templates/TArray.h"

class ZBitArray
{
private:
	TArray<unsigned char> m_aBytes;
	unsigned int m_nSize;
};
