#pragma once

#include "Hook.h"
#include "Glacier/TArray.h"
#include "Glacier/ZResource.h"

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
class ZEntityType;
class ZEntityManager;
class IEntityFactory;
class ZTemplateEntityBlueprintFactory;
struct STemplateEntityBlueprint;
class ZResourcePending;
template<typename T> class TSharedPointer;
class ZResourceDataBuffer;
class ZHeaderLibraryInstaller;
class ZResourceLibraryInfo;
class ZResourceLibraryLoader;
class ZBufferBlock;
struct SResourceLibraryEntry;
class IResourceInstaller;
class ZRuntimeResourceID;
class ZHM5ReloadController;
class ZRenderGBuffer;
class ZHM5MainCamera;
struct SGameUpdateEvent;
class ZRenderPostfilterParametersEntity;
struct SRenderPostfilterParametersColorCorrection;
struct SRenderPostfilterParametersMisc;
class ZCameraEntity;
class ZTossGeometry;
struct float4;
class ZLevelDescriptor;
class ZMenuManager;
enum EMenuStartupState;
class ZLevelSelectManager;

class HitmanAbsolutionSDK_API Hooks
{
  public:
    static StdcallHook<LRESULT(ZApplicationEngineWin32* th, HWND hWnd, UINT uMsgId, WPARAM wParam, LPARAM lParam)>*
        ZApplicationEngineWin32_MainWindowProc;

    static ThiscallHook<void(ZRenderDevice* th)>* ZRenderDevice_Present;

    static ThiscallHook<void(ZRenderSwapChain* th, const SRenderDestinationDesc* pDescription)>* ZRenderSwapChain_Resize;

    static ThiscallHook<bool(ZHitman5Module* th)>* ZHitman5Module_Initialize;

    static ThiscallHook<void(ZEntitySceneContext* th, const ZString& sStreamingState)>* ZEntitySceneContext_CreateScene;

    static ThiscallHook<void(ZEntitySceneContext* th, bool bFullyUnloadScene)>* ZEntitySceneContext_ClearScene;

    static ThiscallHook<void(ZFreeCameraControlEntity* th, float dt)>* ZFreeCameraControlEntity_UpdateCamera;

    static ThiscallHook<void(ZFreeCameraControlEntity* th)>* ZFreeCameraControlEntity_UpdateMovementFromInput;

    static ThiscallHook<bool(ZEngineAppCommon* th, const SRenderDestinationDesc& description)>* ZEngineAppCommon_Initialize;

    static ThiscallHook<void(ZHitman5* th, const ZString& sSubset)>* ZHitman5_Activate;

    static ThiscallHook<void(ZMouseWindows* th, bool bIgnoreOldEvents)>* ZMouseWindows_Update;

    static ThiscallHook<void(ZKeyboardWindows* th, bool bIgnoreOldEvents)>* ZKeyboardWindows_Update;

    static ThiscallHook<ZEntityType**(ZEntityManager* th, const ZString& sDebugName, IEntityFactory* pEntityFactory, uint8_t* pMemBlock)>*
        ZEntityManager_ConstructUninitializedEntity;

    static ThiscallHook<ZTemplateEntityBlueprintFactory*(
        ZTemplateEntityBlueprintFactory* th, STemplateEntityBlueprint* pTemplateEntityBlueprint, ZResourcePending& ResourcePending
    )>* ZTemplateEntityBlueprintFactory_ZTemplateEntityBlueprintFactory;

    static ThiscallHook<bool(ZHeaderLibraryInstaller* th, ZResourcePending& ResourcePending)>* ZHeaderLibraryInstaller_Install;

    static ThiscallHook<bool(ZResourceLibraryLoader* th, ZBufferBlock* pBlock)>* ZResourceLibraryLoader_ProcessBlock;

    static ThiscallHook<void(ZResourceLibraryLoader* th, IResourceInstaller* pInstaller, uint32_t nSize, ZRuntimeResourceID ridResource)>*
        ZResourceLibraryLoader_AllocateEntry;

    static ThiscallHook<void(ZResourceLibraryLoader* th)>* ZResourceLibraryLoader_StartLoading;

    static ThiscallHook<void(ZEngineAppCommon* th)>* ZEngineAppCommon_ResetSceneCallback;

    static ThiscallHook<void(ZHM5ReloadController* th)>* ZHM5ReloadController_EndReloadWeapon;

    static ThiscallHook<void(
        ZRenderPostfilterParametersEntity* th, SRenderPostfilterParametersColorCorrection* parameters, SRenderPostfilterParametersMisc* miscParams
    )>* ZRenderPostfilterParametersEntity_UpdateParametersColorCorrection;

    static ThiscallHook<void(ZCameraEntity* th, float fFovYDeg)>* ZCameraEntity_SetFovYDeg;

    static ThiscallHook<void(ZEngineAppCommon* th)>* ZEngineAppCommon_Uninitialize;

    // static ThiscallHook<void, ZLevelDescriptor> ZLevelDescriptor_Init;

    static ThiscallHook<void(ZMenuManager* th, EMenuStartupState eNewState)>* ZMenuManager_SetStartupState;

    static ThiscallHook<ZRuntimeResourceID*(ZLevelSelectManager* th, ZRuntimeResourceID& result)>* ZLevelSelectManager_GetBootMovie;
};
