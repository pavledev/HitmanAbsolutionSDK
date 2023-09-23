#pragma once

#include "ZInputDevice.h"

class ZMouseDevice : public ZInputDevice
{
protected:
    bool m_bShowCursor;
};

static_assert(sizeof(ZMouseDevice) == 0x88);
static_assert(alignof(ZMouseDevice) == 0x8);
