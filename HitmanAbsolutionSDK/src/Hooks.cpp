#include "Hooks.h"
#include "HookImpl.h"

std::unordered_set<HookBase*>* HookRegistry::g_Hooks = nullptr;

DetourTrampoline* Trampolines::g_Trampolines = nullptr;
size_t Trampolines::g_TrampolineCount = 0;

PATTERN_STDCALL_HOOK(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x56\x8B\x75\x10\x57\x83\xFE", "xxxxx?xxxxxxxx", ZApplicationEngineWin32_MainWindowProc,
    LRESULT(ZApplicationEngineWin32* th, HWND hWnd, UINT uMsgId, WPARAM wParam, LPARAM lParam)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x56\x8B\xF1\xFF\x86\x9C\x00\x00\x00", "xxxxx?xxxxxxxxxx", ZRenderDevice_Present, void(ZRenderDevice* th)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x6A\x00\x68\x00\x00\x00\x00\x64\xA1\x00\x00\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x83\xEC\x00\x53\x56\x57\x89\x65\xF0\x8B\xD9"
    "\xE8",
    "xxxx?x????xx????xxxx????xx?xxxxxxxxx", ZRenderSwapChain_Resize, void(ZRenderSwapChain* th, const SRenderDestinationDesc* pDescription)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x57\x8B\xF1\xE8\x00\x00\x00\x00\x50\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x33\xFF",
    "xxxxx?xxxxx????xx????x????xx", ZHitman5Module_Initialize, bool(ZHitman5Module* th)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x8B\xF1\x57\x8D\x4D\xFC", "xxxxx?xxxxxxx", ZEntitySceneContext_CreateScene,
    void(ZEntitySceneContext* th, const ZString& sStreamingState)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x53\x56\x8B\xF1\x8B\x0D", "xxxxxxxxx", ZEntitySceneContext_ClearScene, void(ZEntitySceneContext* th, bool bFullyUnloadScene)
);

PATTERN_THISCALL_HOOK(
    "\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x56\x8B\xF1\x83\x7E\x10",
    "xxxxx?xx?xx?xxxxxxxxxxxx????xxxxxx", ZFreeCameraControlEntity_UpdateCamera, void(ZFreeCameraControlEntity* th, float dt)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x33\xC0\x83\xEC\x00\x56\x8B\xF1\x89\x46\x24", "xxxxxxx?xxxxxx", ZFreeCameraControlEntity_UpdateMovementFromInput,
    void(ZFreeCameraControlEntity* th)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x33\xDB\x56\x8B\xF1\x89\x5D\xF8", "xxxxx?xxxxxxxxx", ZEngineAppCommon_Initialize,
    bool(ZEngineAppCommon* th, const SRenderDestinationDesc& description)
);

PATTERN_THISCALL_VTABLE_HOOK(
    "\xC7\x07\x00\x00\x00\x00\xC7\x47\x08\x00\x00\x00\x00\xC7\x47\x0C\x00\x00\x00\x00\xC7\x47\x10\x00\x00\x00\x00\xC7\x47\x14",
    "xx????xxx????xxx????xxx????xxx", 6, ZHitman5_Activate, void(ZHitman5* th, const ZString& sSubset)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\xA1\x00\x00\x00\x00\x83\xEC\x00\x56\x8B\xF1\x57", "xxxx????xx?xxxx", ZMouseWindows_Update,
    void(ZMouseWindows* th, bool bIgnoreOldEvents)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x83\xEC\x00\xF6\x05\x00\x00\x00\x00\x00\x56\x8B\xF1", "xxxxx?xx?????xxx", ZKeyboardWindows_Update,
    void(ZKeyboardWindows* th, bool bIgnoreOldEvents)
);

PATTERN_THISCALL_HOOK(
    "\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x56\x8B\x73\x0C\x8B\x06\x8B\x50"
    "\x1C",
    "xxxxx?xx?xx?xxxxxxxxxxxx????xxxxxxxxx", ZEntityManager_ConstructUninitializedEntity,
    ZEntityType**(ZEntityManager* th, const ZString& sDebugName, IEntityFactory* pEntityFactory, uint8_t* pMemBlock)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x56\x8B\xF1\x8B\x4D\x0C\x57\x33\xFF", "xxxxx?xxxxxxxxxx",
    ZTemplateEntityBlueprintFactory_ZTemplateEntityBlueprintFactory,
    ZTemplateEntityBlueprintFactory*(
        ZTemplateEntityBlueprintFactory* th, STemplateEntityBlueprint* pTemplateEntityBlueprint, ZResourcePending& ResourcePending
    )
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x8B\x5D\x08\x56\x8B\xF1\x8B\x4B\x04\x57\xE8\x00\x00\x00\x00\x8B\xF8\x8D\x45\xF4", "xxxxx?xxxxxxxxxxxx????xxxxx",
    ZHeaderLibraryInstaller_Install, bool(ZHeaderLibraryInstaller* th, ZResourcePending& ResourcePending)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x8B\x45\x08\x83\xEC\x00\x53\x8B\x58\x08", "xxxxxxxx?xxxx", ZResourceLibraryLoader_ProcessBlock,
    bool(ZResourceLibraryLoader* th, ZBufferBlock* pBlock)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x56\x57\x8B\x7D\x08\x8B\x07\x8B\x50\x28", "xxxxxxxxxxxxx", ZResourceLibraryLoader_AllocateEntry,
    void(ZResourceLibraryLoader* th, IResourceInstaller* pInstaller, uint32_t nSize, ZRuntimeResourceID ridResource)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x56\x57\x8B\xF9\x8B\x4F\x18", "xxxxx?xxxxxxxx", ZResourceLibraryLoader_StartLoading,
    void(ZResourceLibraryLoader* th)
);

PATTERN_THISCALL_HOOK("\x56\x57\x8B\xF1\x33\xFF\xC6\x46\x44", "xxxxxxxxx", ZEngineAppCommon_ResetSceneCallback, void(ZEngineAppCommon* th));

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x57\x8B\xF1\x80\x66\x50", "xxxxx?xxxxxxx", ZHM5ReloadController_EndReloadWeapon, void(ZHM5ReloadController* th)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x8A\x41\x78", "xxxxxx", ZRenderPostfilterParametersEntity_UpdateParametersColorCorrection,
    void(ZRenderPostfilterParametersEntity* th, SRenderPostfilterParametersColorCorrection* parameters, SRenderPostfilterParametersMisc* miscParams)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\xD9\x45\x08\xD9\xC0\xD9\x41\x68", "xxxxxxxxxxx", ZCameraEntity_SetFovYDeg, void(ZCameraEntity* th, float fFovYDeg)
);

PATTERN_THISCALL_HOOK("\x55\x8B\xEC\x83\xEC\x00\x56\x8B\xF1\x80\x7E\x50", "xxxxx?xxxxxx", ZEngineAppCommon_Uninitialize, void(ZEngineAppCommon* th));

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x8B\x45\x08\x83\xEC\x00\x56\x8B\xF1\x3B\x86\x78\x02\x00\x00", "xxxxxxxx?xxxxxxxxx", ZMenuManager_SetStartupState,
    void(ZMenuManager* th, EMenuStartupState eNewState)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x83\xEC\x00\x56\xC7\x45\xF8", "xxxxx?xxxx", ZLevelSelectManager_GetBootMovie,
    ZRuntimeResourceID*(ZLevelSelectManager* th, ZRuntimeResourceID& result)
);

PATTERN_THISCALL_HOOK(
    "\x55\x8B\xEC\x51\x8B\x45\x08\x56\x57\x89\x4D\xFC\x8D\xB0\x60\x01\x00\x00", "xxxxxxxxxxxxxxxxxx", ZRenderGraphNodeRenderer_DrawOSDAndScaleform,
    void(
        ZRenderGraphNodeRenderer* th, ZRenderContext* pRenderContext, ZRenderTargetView* pRTV, ZRenderDepthStencilView* pDSV,
        TArray<ZRenderGraphNode*>& OSDNodes, ZRenderGraphTraversal* pRenderGraphTraversal
    )
);
