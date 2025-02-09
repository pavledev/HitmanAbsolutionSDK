#pragma once

#include "ZConfigIntBase.h"

class ZConfigInt : public ZConfigIntBase
{
public:
    int GetVal() const
    {
        return m_Value;
    }
};
