#pragma once

#include <unordered_map>
#include <string>

#include "Common.h"

class ZApplicationEngineWin32;
class ZMemoryManager;
class ZRenderManager;
class ZLevelManager;
class ZGraphicsSettingsManager;
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
class ZContentKitManager;
class ZResourceManager;
class ZActorManager;
class ZCheckPointManager;
class ZHM5ActionManager;
class ZEntityManager;
class ZGlobalBoneRegistry;
template<typename TKey> class TDefaultHashMapPolicy;
template<typename TKey, typename TValue, typename THashMapPolicy> class THashMap;
class ZHM5CCProfile;
class LocalResourceIDsResolver;

class HitmanAbsolutionSDK_API Globals
{
  public:
    static ZApplicationEngineWin32** ApplicationEngineWin32;
    static ZMemoryManager** MemoryManager;
    static ZRenderManager* RenderManager;
    static ZLevelManager* LevelManager;
    static ZGraphicsSettingsManager* GraphicsSettingsManager;
    static ZGameTimeManager* GameTimeManager;
    static ZInputDeviceManagerWindows* InputDeviceManager;
    static ZInputActionManager* InputActionManager;
    static ZHitman5Module* Hitman5Module;
    static ZGameLoopManager* GameLoopManager;
    static ZGameWideUI* GameWideUI;
    static ZHUDManager* HUDManager;
    static ZScaleformManager* ScaleformManager;
    static ZInputAction* HM5InputControl;
    static ZCollisionManager* CollisionManager;
    static ZTypeRegistry** TypeRegistry;
    static ZContentKitManager* ContentKitManager;
    static ZResourceManager* ResourceManager;
    static ZActorManager* ActorManager;
    static ZCheckPointManager* CheckPointManager;
    static ZHM5ActionManager* HM5ActionManager;
    static ZEntityManager* EntityManager;
    static ZGlobalBoneRegistry* GlobalBoneRegistry;
    static void* ZTemplateEntityFactoryVFTbl;
    static void* ZTemplateEntityBlueprintFactoryVFTbl;
    static void* ZAspectEntityFactoryVFTbl;
    static void* ZAspectEntityBlueprintFactoryVFTbl;
    static int32_t* GodMode;
    static int32_t* Invisible;
    static ZHM5CCProfile** PresetCCProfiles;
    static int32_t* UIDisableHUD;
    static LocalResourceIDsResolver** LocalResourceResolver;
};
