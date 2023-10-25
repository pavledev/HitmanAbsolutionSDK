#pragma once

#include "TAlignedType.h"

template <typename A, int B>
struct ZMaxArrayData
{
	union
	{
		unsigned char m_data[B * sizeof(A)];
		TAlignedType<alignof(A)> alignDummy;
	};
};
