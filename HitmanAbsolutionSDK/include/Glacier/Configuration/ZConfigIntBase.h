#pragma once

#include "ZConfigCommand.h"

class ZConfigIntBase : public ZConfigCommand
{
public:
    int GetVal() const
    {
        return m_Value;
    }

protected:
    int m_Value;
};
