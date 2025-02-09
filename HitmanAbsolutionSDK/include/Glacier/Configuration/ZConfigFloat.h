#pragma once

#include "ZConfigFloatBase.h"

class ZConfigFloat : public ZConfigFloatBase
{
public:
	float GetVal() const
	{
		return m_Value;
	}
};
