#pragma once

#include <unordered_map>
#include <string>

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

inline std::unordered_map<std::string, uintptr_t> moduleBaseAddresses;

inline ZRenderManager* renderManager;
inline ZLevelManager* levelManager;
inline ZGraphicsSettingsManager* graphicsSettingsManager;
inline ZMemoryManager* memoryManager;
inline ZGameTimeManager* gameTimeManager;
inline ZInputDeviceManagerWindows* inputDeviceManager;
inline ZInputActionManager* inputActionManager;
inline ZHitman5Module* hitman5Module;
