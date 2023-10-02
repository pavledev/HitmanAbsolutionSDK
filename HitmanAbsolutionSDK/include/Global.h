#pragma once

#include <unordered_map>
#include <string>

#include "Common.h"

class DebugConsole;
class ZRenderManager;
class ZLevelManager;
class ZGraphicsSettingsManager;
class ZMemoryManager;
class PipeServer;
class SharedMemoryServer;
class EngineController;
class ZGameTimeManager;
class ZInputDeviceManagerWindows;
class ZInputActionManager;
class ZHitman5Module;
class ZGameLoopManager;
class ZGameWideUI;
class ZHUDManager;
class ZScaleformManager;
class ZInputAction;
class ZCollisionManager;
class ZTypeRegistry;

extern HitmanAbsolutionSDK_API uintptr_t BaseAddress;
extern HitmanAbsolutionSDK_API ZRenderManager* RenderManager;
extern HitmanAbsolutionSDK_API ZLevelManager* LevelManager;
extern HitmanAbsolutionSDK_API ZGraphicsSettingsManager* GraphicsSettingsManager;
extern HitmanAbsolutionSDK_API ZMemoryManager* MemoryManager;
extern HitmanAbsolutionSDK_API ZGameTimeManager* GameTimeManager;
extern HitmanAbsolutionSDK_API ZInputDeviceManagerWindows* InputDeviceManager;
extern HitmanAbsolutionSDK_API ZInputActionManager* InputActionManager;
extern HitmanAbsolutionSDK_API ZHitman5Module* Hitman5Module;
extern HitmanAbsolutionSDK_API ZGameLoopManager* GameLoopManager;
extern HitmanAbsolutionSDK_API ZGameWideUI* GameWideUI;
extern HitmanAbsolutionSDK_API ZHUDManager* HUDManager;
extern HitmanAbsolutionSDK_API ZScaleformManager* ScaleformManager;
extern HitmanAbsolutionSDK_API ZInputAction* HM5InputControl;
extern HitmanAbsolutionSDK_API ZCollisionManager* CollisionManager;
extern HitmanAbsolutionSDK_API ZTypeRegistry** TypeRegistry;
