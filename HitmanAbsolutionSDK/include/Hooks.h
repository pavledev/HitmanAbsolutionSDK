#pragma once

#include "Hook.h"

class ZApplicationEngineWin32;
class ZRenderSwapChain;
class ZRenderDevice;
struct SRenderDestinationDesc;
class ZInputDeviceManager;
class ZVirtualControlWindows;
class ZEngineAppCommon;
class ZHitman5Module;
class ZInputAction;
class ZEntitySceneContext;
class ZString;
class ZFreeCameraControlEntity;
class ZIniFile;
class ZHitman5;
class ZComponentCreateInfo;
class ZMouseWindows;
class ZKeyboardWindows;

namespace Hooks
{
	inline StdCallHook<long, ZApplicationEngineWin32*, HWND, unsigned int, unsigned int, long> ZApplicationEngineWin32_MainWindowProc;
	inline ThisCallHook<void, ZRenderDevice> ZRenderDevice_PresentHook;
	inline ThisCallHook<void, ZRenderSwapChain, const SRenderDestinationDesc*> ZRenderSwapChain_ResizeHook;
	inline StdCallHook<BOOL, const RECT*> ClipCursor;
	inline ThisCallHook<bool, ZHitman5Module> ZHitman5Module_Initialize;
	inline ThisCallHook<bool, ZInputAction> ZInputAction_Digital;
	inline ThisCallHook<void, ZEntitySceneContext, const ZString&> ZEntitySceneContext_CreateScene;
	inline ThisCallHook<void, ZEntitySceneContext, bool> ZEntitySceneContext_ClearScene;
	inline ThisCallHook<void, ZFreeCameraControlEntity, float> ZFreeCameraControlEntity_UpdateCamera;
	inline ThisCallHook<void, ZFreeCameraControlEntity> ZFreeCameraControlEntity_UpdateMovementFromInput;
	inline ThisCallHook<bool, ZInputActionManager, const char*> ZInputActionManager_AddBindings;
	inline ThisCallHook<bool, ZEngineAppCommon, const SRenderDestinationDesc&> ZEngineAppCommon_Initialize;
	inline ThisCallHook<void, ZHitman5, ZComponentCreateInfo&> ZHitman5_ZHitman5;
	inline ThisCallHook<void, ZMouseWindows, bool> ZMouseWindows_Update;
	inline ThisCallHook<void, ZKeyboardWindows, bool> ZKeyboardWindows_Update;
}
