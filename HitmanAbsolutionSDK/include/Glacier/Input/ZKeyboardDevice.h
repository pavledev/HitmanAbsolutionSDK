#pragma once

#include "ZInputDevice.h"

class ZKeyboardDevice : public ZInputDevice
{
    virtual ~ZKeyboardDevice() = 0;
};

static_assert(sizeof(ZKeyboardDevice) == 0x80);
static_assert(alignof(ZKeyboardDevice) == 0x8);
