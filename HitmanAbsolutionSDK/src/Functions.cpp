#include "Functions.h"
#include "EngineFunctionImpl.h"

PATTERN_CDECL_FUNCTION(
    "\x55\x8B\xEC\x51\x8B\x45\x08\x8B\x55\x0C", "xxxxxxxxxx", SetPropertyValue,
    bool(ZEntityType** pEntity, uint32_t nPropertyID, const ZVariantRef& value, bool bInvokeChangeHandlers)
);

PATTERN_CDECL_FUNCTION(
    "\x55\x8B\xEC\x51\x8B\x45\x08\x53\x8B\x5D\x0C\x56\x8B\x30\x8D\x4D\xFC\x51\x53\x8B\xCE\xC7\x45\xFC\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x84\xC0\x74"
    "\x00\x8B\x4D\xFC\x8B\x56\x14",
    "xxxxxxxxxxxxxxxxxxxxxxxx????x????xxx?xxxxxx", SignalInputPin, void(ZEntityType** pEntity, uint32_t nPinID, const ZVariantRef& data)
);

PATTERN_CDECL_FUNCTION(
    "\x55\x8B\xEC\x51\x8B\x45\x08\x53\x8B\x5D\x0C\x56\x8B\x30\x8D\x4D\xFC\x51\x53\x8B\xCE\xC7\x45\xFC\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x84\xC0\x74"
    "\x00\x8B\x4D\xFC\x8B\x56\x18",
    "xxxxxxxxxxxxxxxxxxxxxxxx????x????xxx?xxxxxx", SignalOutputPin, void(ZEntityType** pEntity, uint32_t nPinID, const ZVariantRef& data)
);

PATTERN_CDECL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\xE8\x00\x00\x00\x00\x8B\xC8\xE8\x00\x00\x00\x00\x8B\x4D\x08", "xxxxx?x????xxx????xxx", GetApplicationOptionBool,
    bool(const ZString& sName, bool bDefault)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x8B\xF1\x8B\x86\x28\x03\x00\x00", "xxxxx?xxxxxxxxx", ZActor_KillActor,
    void(ZActor* th, EActorDeathType eDeathType, bool bDeathVisible)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x8B\x01\x8B\x90\x90\x00\x00\x00", "xxxxxxxxxxx", ZActor_GetBoneWorldPosition,
    float4*(const ZActor* th, float4& result, uint32_t iBoneId)
);

PATTERN_THISCALL_FUNCTION(
    "\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x56\x8B\xF1\x8B\x46\x64\x83"
    "\xE8",
    "xxxxx?xx?xx?xxxxxxxxxxxx????xxxxxxxx", ZCameraEntity_UpdateProjection, void(ZCameraEntity* th)
);

PATTERN_THISCALL_FUNCTION("\x55\x8B\xEC\xD9\x45\x08\xD9\x41\x78", "xxxxxxxxx", ZCameraEntity_SetNearZ, void(ZCameraEntity* th, float fNearZ));

PATTERN_THISCALL_FUNCTION("\x55\x8B\xEC\xD9\x45\x08\xD9\x41\x7C", "xxxxxxxxx", ZCameraEntity_SetFarZ, void(ZCameraEntity* th, float fFarZ));

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x8A\x45\x08\x56\x8B\xF1\x88\x46\x0C", "xxxxxxxxxxxx", ZFreeCameraControlEntity_SetActive,
    void(ZFreeCameraControlEntity* th, bool bActive)
);

PATTERN_THISCALL_FUNCTION(
    "\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x8B\x53\x18", "xxxxx?xx?xx?xxxxxxxxxxxxx",
    ZFreeCameraControlEntity_GetUpdatedCameraPosition,
    float4*(ZFreeCameraControlEntity* th, float4& result, float fMoveX, float fMoveY, float fMoveZ, const SMatrix& mCurrentCameraToWorld)
);

PATTERN_THISCALL_FUNCTION(
    "\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x43\x08\x0F\x28\x2D",
    "xxxxx?xx?xx?xxxxxxxxxxxx????xxxxxx", ZFreeCameraControlEntity_GetUpdatedCameraRotation,
    SMatrix*(
        ZFreeCameraControlEntity* th, SMatrix& result, float fDeltaRoll, float fDeltaPitch, float fDeltaYaw, const SMatrix& mCurrentCameraToWorld
    )
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x8B\x89\xB4\x01\x00\x00\x5D\xE9\x00\x00\x00\x00\x00\x55", "xxxxxxxxxxx?????x", ZHM5MainCamera_SetCameraDirection,
    void(ZHM5MainCamera* th, const float4& vLookTo)
);

PATTERN_THISCALL_FUNCTION(
    "\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x8B\x41\x54", "xxxxx?xx?xx?xxxxxxxxxxxxx",
    ZCheckPointManagerEntity_ActivateJumpPoint, void(ZCheckPointManagerEntity* th, int32_t iIndex, bool bResetHitman)
);

PATTERN_CDECL_FUNCTION(
    "\x56\xE8\x00\x00\x00\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x00\x8B\xF0\xE8\x00\x00\x00\x00\x8B\xC6",
    "xx????x????x????xx?xxx????xx", ZConfigCommand_First, ZConfigCommand*()
);

PATTERN_CDECL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x56\xE8\x00\x00\x00\x00\x68", "xxxxx?xx????x", ZConfigCommand_ExecuteCommand,
    void(const ZString& pCommandName, const char* argv)
);

PATTERN_THISCALL_FUNCTION(
    "\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x56\x57\x8B\xF1\x8B\x0D",
    "xxxxx?xx?xx?xxxxxxxxxxxx????xxxxxx", ZEngineAppCommon_CreateFreeCameraAndControl, void(ZEngineAppCommon* th)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\x53\x56\x57\x6A\x00\x8D\x45\xE4", "xxxxx????xxxx?xxx", ZIniFile_LoadFromStringInternal,
    bool(ZIniFile* th, const ZString& sIniFileContent, const ZFilePath& path)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x56\x57\x8D\x55\xF0", "xxxxx?xxxxxx", ZWin32ApplicationStub_ApplyOptionOverrides,
    void(ZWin32ApplicationStub* th, int32_t& argc, char** argv)
);

PATTERN_THISCALL_FUNCTION("\x55\x8B\xEC\x51\xA1\x00\x00\x00\x00\x57", "xxxxx????x", ZInputAction_Analog, float(ZInputAction* th));

PATTERN_THISCALL_FUNCTION(
    "\xA1\x00\x00\x00\x00\x57\x8B\xF9\x3B\x47\x08\x74\x00\x8B\x0F\x51\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x89\x47\x04\x8B\x15\x00\x00\x00\x00\x89"
    "\x57\x08\x8B\x4F\x04\x85\xC9\x75\x00\x32\xC0",
    "x????xxxxxxx?xxxx????x????xxxxx????xxxxxxxxx?xx", ZInputAction_Digital, bool(ZInputAction* th)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\xA1\x00\x00\x00\x00\x56\x8B\xF1\x3B\x46\x08\x74\x00\x8B\x0E\x51\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x89\x46\x04\x8B\x15\x00\x00"
    "\x00\x00\x89\x56\x08\x8B\x76\x04",
    "xxxx????xxxxxxx?xxxx????x????xxxxx????xxxxxx", ZInputAction_SetEnabled, void(ZInputAction* th, bool bEnabled)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x8B\x55\x08\x81\xEC\x00\x00\x00\x00\x53", "xxxxxxxx????x", ZInputActionManager_AddBindings,
    bool(ZInputActionManager* th, const char* binds)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x8B\x75\x08\x33\xC0", "xxxxx?xxxxxx", ZHitman5_EquipOutfitResource,
    void(ZHitman5* th, ZRuntimeResourceID ridNewOutfitLibrary, TEntityRef<IHM5Outfit> Outfit, bool bPlaceClothBundle, bool bForce)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x8B\x75\x08\x83\x7E\x04\x00\x57\x8B\xF9\x75\x00\x5F\x33\xC0", "xxxxx?xxxxxxx?xxxx?xxx",
    ZHM5BaseInventory_AddItemToInventory,
    ZInventorySlot*(ZHM5BaseInventory* th, const TEntityRef<IHM5Item>& rItem, bool bAddInitialBullets, bool bDisableHUD)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x8B\x0D\x00\x00\x00\x00\x83\xEC\x00\x56\xE8\x00\x00\x00\x00\x33\xF6\x3B\xC6\x75\x00\x8B\x45\x08\x89\x70\x04\x89\x30\x5E\x8B\xE5\x5D"
    "\xC2\x00\x00\x8B\x48\x0C\x3B\xCE\x74\x00\x8B\x01\x8B\x40\x14\x8D\x55\xFC\x52\xFF\xD0\x89\x75\xF4\x8B\x08\x89\x4D\xF4\x68\x00\x00\x00\x00\x8B\xC8"
    "\xE8\x00\x00\x00\x00\x8B\x75\xF4\x68\x00\x00\x00\x00\x8D\x4D\xF4\x89\x45\xF8\xE8\x00\x00\x00\x00\x8B\xC8\x8B\x45\x08\x89\x30\x89\x48\x04\x5E\x8B"
    "\xE5\x5D\xC2\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x55",
    "xxxxx????xx?xx????xxxxx?xxxxxxxxxxxxx??xxxxxx?xxxxxxxxxxxxxxxxxxxx????xxx????xxxx????xxxxxxx????xxxxxxxxxxxxxxx?????????????????x",
    ZRenderManager_GetGameRenderDestinationEntity,
    TEntityRef<IRenderDestinationEntity>*(ZRenderManager* th, TEntityRef<IRenderDestinationEntity>& result)
);

PATTERN_RELATIVE_THISCALL_FUNCTION(
    "\xE8\x00\x00\x00\x00\x8D\x45\xF8\x50\x8B\xCE\xE8\x00\x00\x00\x00\x68\x00\x00\x00\x00\x8B\xC8\xE8\x00\x00\x00\x00\xC7\x45\xF8\x00\x00\x00\x00\x85"
    "\xC0\x74\x00\x8D\x4D\xB0\x51\x8B\xC8\xE8\x00\x00\x00\x00\x8B\x43\x08\x8B\x55\xE0",
    "x????xxxxxxx????x????xxx????xxx????xxx?xxxxxxx????xxxxxx", ZRenderManager_GetActiveRenderDestinationEntity,
    TEntityRef<IRenderDestinationEntity>*(const ZRenderManager* th, TEntityRef<IRenderDestinationEntity>& result)
);

PATTERN_RELATIVE_THISCALL_FUNCTION(
    "\xE8\x00\x00\x00\x00\x8D\x45\x08\x50\x8B\xCB\xE8\x00\x00\x00\x00\x8D\x4D\x08", "x????xxxxxxx????xxx", ZResourcePtr_AddStatusChangedListener,
    void(ZResourcePtr* th, const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>& d)
);

PATTERN_RELATIVE_THISCALL_FUNCTION(
    "\xE8\x00\x00\x00\x00\x80\x66\x30\x00\x8D\x4E\x0C", "x????xxx?xxx", ZResourcePtr_RemoveStatusChangedListener,
    void(ZResourcePtr* th, const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>& d)
);

PATTERN_RELATIVE_THISCALL_FUNCTION(
    "\xE8\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x33\xC9\x89\x45\xF0\x89\x4D\xF4\x6A\x00\x8D\x45\xF0\x50\xB9\x00\x00\x00\x00\x89\x75\xF4\xE8\x00\x00\x00"
    "\x00\x8D\x4D\xF8",
    "x????x????xxxxxxxxx?xxxxx????xxxx????xxx", ZGameLoopManager_RegisterForFrameUpdate,
    void(ZGameLoopManager* th, const ZDelegate<void __cdecl(SGameUpdateEvent const&)>& callback, int32_t nPriority)
);

PATTERN_RELATIVE_THISCALL_FUNCTION(
    "\xE8\x00\x00\x00\x00\x80\xA6\xF8\x02\x00\x00\x00\x33\xC9", "x????xxxxxx?xx", ZGameLoopManager_UnregisterForFrameUpdate,
    void(ZGameLoopManager* th, const ZDelegate<void __cdecl(SGameUpdateEvent const&)>& callback)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x8B\x45\x08\x48\x56", "xxxxxxxx", ZGameLoopManager_SetPlayMode, void(ZGameLoopManager* th, EPlayMode ePlayMode)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x56\x57\x8B\x7D\x0C\x33\xC0", "xxxxx?xxxxxxxx", ZResourceManager_GetResourcePtr,
    ZResourcePtr*(ZResourceManager* th, ZResourcePtr& result, const ZRuntimeResourceID& ridResource, int32_t nPriority)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x56\x57\x89\x4D\xF8\xE8\x00\x00\x00\x00\xE8", "xxxxx?xxxxxxx????x", ZResourceManager_LoadResource,
    ZResourcePtr*(ZResourceManager* th, ZResourcePtr& result, const ZRuntimeResourceID& ridResource)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x8B\xF1\xE8\x00\x00\x00\x00\x83\xBE\x14\x01\x00\x00", "xxxxx?xxxx????xxxxxx", ZResourceManager_Update,
    void(ZResourceManager* th, bool bSendStatusChangedNotifications)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x56\x57\x8B\xF9\x8D\x9F\xAC\x01\x00\x00", "xxxxx?xxxxxxxxxxx", ZResourceManager_ReleaseStub,
    void(ZResourceManager* th, ZResourceStub* stub)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x56\x8B\xF1\x57\x8D\xBE\xAC\x01\x00\x00", "xxxxxxxxxxxxx", ZResourceManager_GetResourceInstaller,
    IResourceInstaller*(const ZResourceManager* th, uint32_t nResourceType)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x8B\x5D\x08\x56\x03\xDB", "xxxxx?xxxxxxx", ZResourceLibraryInfo_InstallResource,
    bool(ZResourceLibraryInfo* th, uint32_t nIndex, TSharedPointer<ZResourceDataBuffer> pResourceData)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x53\x56\x57\x8B\xF1\xE8\x00\x00\x00\x00\x8B\x46\x44", "xxxxxxxxx????xxx", ZResourceLibrarySet_RemoveReadyCallback,
    void(ZResourceLibrarySet* th, const ZDelegate<void __cdecl(void)>& d)
);

PATTERN_THISCALL_FUNCTION("\x56\x8B\xF1\xE8\x00\x00\x00\x00\x80\x4E\x54", "xxxx????xxx", ZResourceLibrarySet_Release, void(ZResourceLibrarySet* th));

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x56\x8B\xF1\x33\xDB\x57\x8D\x4E\x0C", "xxxxx?xxxxxxxxxx", ZDynamicResourceLibrary_ZDynamicResourceLibrary,
    void(ZDynamicResourceLibrary* th, const ZRuntimeResourceID& ridHeaderLibrary, bool bDelayedCreateEntities, uint32_t nNumInstances)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x56\x8B\xF1\x8B\x46\x24\x2B\x46\x20", "xxxxx?xxxxxxxxxx", ZDynamicResourceLibrary_CreateEntities,
    void(ZDynamicResourceLibrary* th)
);

PATTERN_THISCALL_FUNCTION(
    "\x53\x56\x57\x8B\xF1\x8B\x7E\x24", "xxxxxxxx", ZDynamicResourceLibrary_DeleteAllEntities, void(ZDynamicResourceLibrary* th)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x8B\xF1\x57\x8D\x4E\x0C", "xxxxx?xxxxxxx", ZDynamicResourceLibrary_OnHeaderLibraryReady,
    void(ZDynamicResourceLibrary* th, const ZRuntimeResourceID& rid)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x8B\xF1\xB8\x00\x00\x00\x00\x33\xC9\x89\x45\xF8\x89\x4D\xFC\x8B\x4E\x08", "xxxxx?xxxx????xxxxxxxxxxx",
    ZDynamicResourceLibrary_OnResourceLibrariesReady, void(ZDynamicResourceLibrary* th)
);

PATTERN_CDECL_FUNCTION(
    "\x55\x8B\xEC\xE8\x00\x00\x00\x00\x50\x6A\x00\xE8\x00\x00\x00\x00\x33\xC9\x83\xC4\x00\x3B\xC1\x74\x00\x8B\x55\x0C\x89\x50\x08\x8B\x55\x10\x89\x48"
    "\x04\xC7\x00\x00\x00\x00\x00\x89\x48\x0C\x89\x50\x10\xC6\x40\x14",
    "xxxx????xx?x????xxxx?xxx?xxxxxxxxxxxxxx????xxxxxxxxx", ZResourceDataBuffer_Create,
    TSharedPointer<ZResourceDataBuffer>*(TSharedPointer<ZResourceDataBuffer>& result, void* data, uint32_t capacity)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x56\x57\x8B\x7D\x08\x8B\x07\x8B\x50\x28", "xxxxxxxxxxxxx", ZResourceLibraryLoader_AllocateEntry,
    void(ZResourceLibraryLoader* th, IResourceInstaller* pInstaller, uint32_t nSize, ZRuntimeResourceID ridResource)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x57\x8B\xF9\x8D\x77\x04\x8B\xCE\xE8\x00\x00\x00\x00\x8B\x45\x08\x50\x8D\x4D\xF8", "xxxxx?xxxxxxxxxx????xxxxxxx",
    ZGlobalBoneRegistry_GetBoneID, int32_t(const ZGlobalBoneRegistry* th, char const* pBoneName)
);

PATTERN_THISCALL_FUNCTION(
    "\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x8B\x41\x08\xF3\x0F\x10\x51\x34\x83\xEC\x00\x56\x57\x85"
    "\xC0\x75\x00\xF3\x0F\x7E\x49\x2C\x0F\x28\x1D\x00\x00\x00\x00\x0F\x10\x41\x1C\x0F\xC6\xCA\x00\x0F\x28\xD1\x0F\xC6\xD9\x00\x0F\xC6\xD3\x00\x0F\x29"
    "\x55\xF0\x8B\x75\xF8",
    "xxxxx?xx?xx?xxxxxxxxxxxxxxxxxxxx?xxxxx?xxxxxxxx????xxxxxxx?xxxxxx?xxx?xxxxxxx", ZSpatialEntity_GetObjectToWorldMatrix,
    SMatrix*(const ZSpatialEntity* th, SMatrix& result)
);

PATTERN_THISCALL_FUNCTION(
    "\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x56\x8B\x73\x08\x57\x8B\xF9\x8B"
    "\x47\x08\x89\x7D\xFC",
    "xxxxx?xx?xx?xxxxxxxxxxxx????xxxxxxxxxxxxx", ZSpatialEntity_SetObjectToWorldMatrix, void(ZSpatialEntity* th, const SMatrix& mObjectToWorld)
);

PATTERN_THISCALL_FUNCTION(
    "\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x8B\x41\x08\x83\xEC", "xxxxx?xx?xx?xxxxxxxxxxxxxxx",
    ZSpatialEntity_GetWorldPosition, float4*(const ZSpatialEntity* th, float4& result)
);

PATTERN_THISCALL_FUNCTION(
    "\x53\x8B\xDC\x83\xEC\x00\x83\xE4\x00\x83\xC4\x00\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x83\xEC\x00\x56\x8D\x45\xC0\x50\x8B\xF1\xE8\x00\x00\x00"
    "\x00\x8B\x43\x08",
    "xxxxx?xx?xx?xxxxxxxxxxxx?xxxxxxxx????xxx", ZSpatialEntity_SetWorldPosition, void(ZSpatialEntity* th, const float4& vWorldPosition)
);

PATTERN_RELATIVE_THISCALL_FUNCTION(
    "\xE8\x00\x00\x00\x00\x8B\x55\xF8\x8A\x42\x20", "x????xxxxxx", GFxValue_GetMember, bool(const GFxValue* th, const char* name, GFxValue* pval)
);

PATTERN_RELATIVE_THISCALL_FUNCTION(
    "\xE8\x00\x00\x00\x00\x8D\x4D\xDC\xE8\x00\x00\x00\x00\x8D\x95\x24\xFF\xFF\xFF", "x????xxxx????xxxxxx", GFxValue_SetMember,
    bool(GFxValue* th, const char* name, const GFxValue& val)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x8A\x45\x08\x83\xEC\x00\x56\x8B\xF1\x83\x7E\x14", "xxxxxxxx?xxxxxx", ZGameWideUIScaleformHandler_ShowUICursor,
    void(ZGameWideUIScaleformHandler* th, bool bShow)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x8B\x45\x08\x83\xEC\x00\x53\x56\x50\x8B\xF1\xE8\x00\x00\x00\x00\x80\x7E\x79", "xxxxxxxx?xxxxxx????xxx", ZHUDManager_ShowHUD,
    void(ZHUDManager* th, bool bShow)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x8A\x5D\x08\x56\x8B\xF1\x3A\x9E\xF0\x2E\x00\x00", "xxxxx?xxxxxxxxxxxxx", ZHUDManager_ShowTrespassingIcon,
    void(ZHUDManager* th, bool bShow)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x8B\xF1\x80\x7E\x79\x00\x0F\x84\x00\x00\x00\x00\x8B\x45\x08", "xxxxx?xxxxxx?xx????xxx", ZHUDManager_ShowAIStateIcon,
    void(ZHUDManager* th, int32_t nState)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x80\x79\x79\x00\x74\x00\x0F\xB6\x45\x08", "xxxxxx?x?xxxx", ZHUDManager_ShowThreatRadar, void(ZHUDManager* th, bool bShow)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x8B\x55\x10\x56\x8B\xF1", "xxxxx?xxxxxx", ZHUDManager_FadeHUDElements,
    void(ZHUDManager* th, uint32_t nElements, EHUDFadeReason eReason, bool bFadeIn, float fTime)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x56\x8B\xF1\x80\x7E\x79\x00\x74\x00\x53", "xxxxx?xxxxxx?x?x", ZHUDManager_ScaleformShowWeaponDisplay,
    void(ZHUDManager* th, bool bShow)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x83\x79\x0C\x00\x0F\x84\x00\x00\x00\x00\x8B\x15\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x89\x55\xF4\x89\x55\xEC\x89\x55\xE4"
    "\x89\x55\xDC\x89\x55\xD4\x89\x55\xCC\x89\x55\xC4\x89\x55\xBC\x89\x55\xB4\x89\x55\xAC\x8B\x49\x0C\x8D\x55\xF0\x52\x8D\x55\xE8\x52\x8D\x55\xE0\x52"
    "\x8D\x55\xD8\x52\x8D\x55\xD0\x52\x8D\x55\xC8\x52\x8D\x55\xC0\x52\x8D\x55\xB8\x52\x8D\x55\xB0\x52\x8D\x55\xA8\x89\x45\xF0\x89\x45\xE8\x89\x45\xE0"
    "\x89\x45\xD8\x89\x45\xD0\x89\x45\xC8\x89\x45\xC0\x89\x45\xB8\x89\x45\xB0\x89\x45\xA8\x52\xC7\x45\xF8\x00\x00\x00\x00\xC7\x45\xFC\x00\x00\x00\x00"
    "\x8B\x01\x8B\x40\x28\x8D\x55\xF8\x52\xFF\xD0\x8D\x4D\xF8\xE8\x00\x00\x00\x00\x8B\xE5\x5D\xC3\x00\x00\x00\x00\x00\x00\x00\x00\x00\x55\x8B\xEC\x83"
    "\xEC",
    "xxxxx?xxx?xx????xx????x????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xxx????"
    "xxxxxxxxxxxxxxx????xxxx?????????xxxxx",
    ZHUDManager_HideRatingTracker, void(ZHUDManager* th)
);

PATTERN_THISCALL_FUNCTION(
    "\x55\x8B\xEC\x51\x53\x56\x57\x8B\xF9\xE8\x00\x00\x00\x00\x8B\x45\x08", "xxxxxxxxxx????xxx", LocalResourceIDsResolver_RecordMapping,
    void(LocalResourceIDsResolver* th, const ZRuntimeResourceID& ridResource, const ZResourceID& idResource)
);

PATTERN_CDECL_FUNCTION(
    "\x55\x8B\xEC\x83\xEC\x00\x53\x56\x57\xE8\x00\x00\x00\x00\x68", "xxxxx?xxxx????x", ZConfigCommand_GetConfigVariable,
    ZConfigCommand*(const ZString& pszName)
);
