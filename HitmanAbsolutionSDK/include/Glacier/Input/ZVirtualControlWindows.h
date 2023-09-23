#pragma once

#include "ZInputDevice.h"
#include "Utility/Alignment.h"

class ZVirtualControlWindows : public ZInputDevice
{
public:
    ZInputDevice* GetKeyboardDevice() const;
    ZInputDevice* GetMouseDevice() const;

private:
    PAD(0x1F8);
    ZInputDevice* m_Keyboard;
    ZInputDevice* m_Mouse;
};

static_assert(sizeof(ZVirtualControlWindows) == 0x280);
static_assert(alignof(ZVirtualControlWindows) == 0x8);
