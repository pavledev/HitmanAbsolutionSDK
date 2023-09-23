#pragma once

#include "Hook.h"

class ZApplicationEngineWin32;
class ZRenderSwapChain;
class ZRenderDevice;
struct SRenderDestinationDesc;
class ZInputDeviceManager;
class ZVirtualControlWindows;
class ZMouseWindows;
class ZEngineAppCommon;
class ZHitman5Module;

namespace Hooks
{
	inline StdCallHook<long, ZApplicationEngineWin32*, HWND, unsigned int, unsigned int, long> ZApplicationEngineWin32_MainWindowProc;
	inline ThisCallHook<void, ZRenderDevice> ZRenderDevice_PresentHook;
	inline ThisCallHook<void, ZRenderSwapChain, const SRenderDestinationDesc*> ZRenderSwapChain_ResizeHook;
	inline StdCallHook<BOOL, const RECT*> ClipCursor;
	inline ThisCallHook<void, ZEngineAppCommon> ZEngineAppCommon_UpdateInputDeviceManager;
	inline ThisCallHook<bool, ZHitman5Module> ZHitman5Module_Initialize;
}
