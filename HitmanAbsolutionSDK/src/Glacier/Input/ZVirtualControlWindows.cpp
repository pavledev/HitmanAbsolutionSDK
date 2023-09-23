#include "Glacier/Input/ZVirtualControlWindows.h"

ZInputDevice* ZVirtualControlWindows::GetKeyboardDevice() const
{
	return m_Keyboard;
}

ZInputDevice* ZVirtualControlWindows::GetMouseDevice() const
{
	return m_Mouse;
}
