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
class ZEntityManager;
class IEntityFactory;
class ZTemplateEntityBlueprintFactory;
struct STemplateEntityBlueprint;
class ZResourcePending;
template <typename T> class TSharedPointer;
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
	inline ThisCallHook<void, ZEngineAppCommon> ZEngineAppCommon_DefaultMainLoopSequence;
	inline ThisCallHook<ZEntityType**, ZEntityManager, const ZString&, IEntityFactory*, unsigned char*> ZEntityManager_ConstructUninitializedEntity;
	inline ThisCallHook<void, ZTemplateEntityBlueprintFactory, STemplateEntityBlueprint*, ZResourcePending&> ZTemplateEntityBlueprintFactory_ZTemplateEntityBlueprintFactory;
	inline ThisCallHook<bool, ZHeaderLibraryInstaller, ZResourcePending&> ZHeaderLibraryInstaller_Install;
	inline ThisCallHook<bool, ZResourceLibraryInfo, unsigned int, TSharedPointer<ZResourceDataBuffer>> ZResourceLibraryInfo_InstallResource;
	inline ThisCallHook<bool, ZResourceLibraryLoader, ZBufferBlock*> ZResourceLibraryLoader_ProcessBlock;
	inline ThisCallHook<SResourceLibraryEntry*, ZResourceLibraryInfo, SResourceLibraryEntry*, unsigned int> ZResourceLibraryInfo_GetEntry;
	inline ThisCallHook<void, ZResourceLibraryLoader, IResourceInstaller*, unsigned int, ZRuntimeResourceID> ZResourceLibraryLoader_AllocateEntry;
	inline ThisCallHook<void, ZResourceLibraryLoader, ZResourcePending&, ZResourceLibraryInfo*> ZResourceLibraryLoader_ZResourceLibraryLoader;
	inline ThisCallHook<void, ZResourceLibraryLoader> ZResourceLibraryLoader_StartLoading;
	inline ThisCallHook<void, ZEngineAppCommon> ZEngineAppCommon_ResetSceneCallback;
	inline ThisCallHook<void, ZHM5ReloadController> ZHM5ReloadController_EndReloadWeapon;
	inline CdeclHook<bool, const ZString&, bool> GetApplicationOptionBool;
	inline ThisCallHook<void, ZRenderGBuffer,
		unsigned int,
		unsigned int,
		ZString,
		ZRenderDevice*,
		unsigned int> ZRenderGBuffer_ZRenderGBuffer;
	inline ThisCallHook<void, ZHM5MainCamera, const SGameUpdateEvent*, bool> ZHM5MainCamera_UpdateMainCamera;
	inline ThisCallHook<void, ZRenderPostfilterParametersEntity, SRenderPostfilterParametersColorCorrection*, SRenderPostfilterParametersMisc*> ZRenderPostfilterParametersEntity_UpdateParametersColorCorrection;
}
