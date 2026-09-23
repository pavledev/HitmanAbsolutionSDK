#pragma once

#include "Glacier/ZMath.h"

#include "Common.h"
#include "EngineFunction.h"

class ZEntityType;
class ZVariantRef;
class ZString;
class ZActor;
enum EActorDeathType;
class ZCameraEntity;
class ZFreeCameraControlEntity;
class ZHM5MainCamera;
class ZCheckPointManagerEntity;
class ZConfigCommand;
class ZEngineAppCommon;
class ZIniFile;
class ZFilePath;
class ZWin32ApplicationStub;
class ZInputAction;
class ZInputActionManager;
class ZHitman5;
class ZRuntimeResourceID;
template<typename T> class TEntityRef;
class IHM5Outfit;
class ZInventorySlot;
class ZHM5BaseInventory;
class IHM5Item;
class IRenderDestinationEntity;
class ZRenderManager;
class ZResourcePtr;
template<typename T> class ZDelegate;
class ZGameLoopManager;
struct SGameUpdateEvent;
enum EPlayMode;
class ZResourceManager;
class ZResourceStub;
class IResourceInstaller;
class ZResourceLibraryInfo;
template<typename T> class TSharedPointer;
class ZResourceDataBuffer;
class ZResourceLibrarySet;
class ZDynamicResourceLibrary;
class ZResourceLibraryLoader;
class ZGlobalBoneRegistry;
class ZSpatialEntity;
class GFxValue;
class ZGameWideUIScaleformHandler;
class ZHUDManager;
enum EHUDFadeReason;
class LocalResourceIDsResolver;
class ZResourceID;

class HitmanAbsolutionSDK_API Functions
{
  public:
    static CdeclEngineFunction<bool(ZEntityType** pEntity, uint32_t nPropertyID, const ZVariantRef& value, bool bInvokeChangeHandlers)>*
        SetPropertyValue;

    static CdeclEngineFunction<void(ZEntityType** pEntity, uint32_t nPinID, const ZVariantRef& data)>* SignalInputPin;

    static CdeclEngineFunction<void(ZEntityType** pEntity, uint32_t nPinID, const ZVariantRef& data)>* SignalOutputPin;

    static CdeclEngineFunction<bool(const ZString& sName, bool bDefault)>* GetApplicationOptionBool;

    static ThiscallEngineFunction<void(ZActor* th, EActorDeathType eDeathType, bool bDeathVisible)>* ZActor_KillActor;

    static ThiscallEngineFunction<float4*(const ZActor* th, float4& result, uint32_t iBoneId)>* ZActor_GetBoneWorldPosition;

    static ThiscallEngineFunction<void(ZCameraEntity* th)>* ZCameraEntity_UpdateProjection;

    static ThiscallEngineFunction<void(ZCameraEntity* th, float fNearZ)>* ZCameraEntity_SetNearZ;

    static ThiscallEngineFunction<void(ZCameraEntity* th, float fFarZ)>* ZCameraEntity_SetFarZ;

    static ThiscallEngineFunction<void(ZFreeCameraControlEntity* th, bool bActive)>* ZFreeCameraControlEntity_SetActive;

    static ThiscallEngineFunction<float4*(
        ZFreeCameraControlEntity* th, float4& result, float fMoveX, float fMoveY, float fMoveZ, const SMatrix& mCurrentCameraToWorld
    )>* ZFreeCameraControlEntity_GetUpdatedCameraPosition;

    static ThiscallEngineFunction<SMatrix*(
        ZFreeCameraControlEntity* th, SMatrix& result, float fDeltaRoll, float fDeltaPitch, float fDeltaYaw, const SMatrix& mCurrentCameraToWorld
    )>* ZFreeCameraControlEntity_GetUpdatedCameraRotation;

    static ThiscallEngineFunction<void(ZHM5MainCamera* th, const float4& vLookTo)>* ZHM5MainCamera_SetCameraDirection;

    static ThiscallEngineFunction<void(ZCheckPointManagerEntity* th, int32_t iIndex, bool bResetHitman)>* ZCheckPointManagerEntity_ActivateJumpPoint;

    static CdeclEngineFunction<ZConfigCommand*()>* ZConfigCommand_First;

    static CdeclEngineFunction<void(const ZString& pCommandName, const char* argv)>* ZConfigCommand_ExecuteCommand;

    static ThiscallEngineFunction<void(ZEngineAppCommon* th)>* ZEngineAppCommon_CreateFreeCameraAndControl;

    static ThiscallEngineFunction<bool(ZIniFile* th, const ZString& sIniFileContent, const ZFilePath& path)>* ZIniFile_LoadFromStringInternal;

    static ThiscallEngineFunction<void(ZWin32ApplicationStub* th, int32_t& argc, char** argv)>* ZWin32ApplicationStub_ApplyOptionOverrides;

    static ThiscallEngineFunction<float(ZInputAction* th)>* ZInputAction_Analog;

    static ThiscallEngineFunction<bool(ZInputAction* th)>* ZInputAction_Digital;

    static ThiscallEngineFunction<void(ZInputAction* th, bool bEnabled)>* ZInputAction_SetEnabled;

    static ThiscallEngineFunction<bool(ZInputActionManager* th, const char* binds)>* ZInputActionManager_AddBindings;

    static ThiscallEngineFunction<void(
        ZHitman5* th, ZRuntimeResourceID ridNewOutfitLibrary, TEntityRef<IHM5Outfit> Outfit, bool bPlaceClothBundle, bool bForce
    )>* ZHitman5_EquipOutfitResource;

    static ThiscallEngineFunction<ZInventorySlot*(
        ZHM5BaseInventory* th, const TEntityRef<IHM5Item>& rItem, bool bAddInitialBullets, bool bDisableHUD
    )>* ZHM5BaseInventory_AddItemToInventory;

    static ThiscallEngineFunction<TEntityRef<IRenderDestinationEntity>*(ZRenderManager* th, TEntityRef<IRenderDestinationEntity>& result)>*
        ZRenderManager_GetGameRenderDestinationEntity;

    static ThiscallEngineFunction<TEntityRef<IRenderDestinationEntity>*(const ZRenderManager* th, TEntityRef<IRenderDestinationEntity>& result)>*
        ZRenderManager_GetActiveRenderDestinationEntity;

    static ThiscallEngineFunction<void(ZResourcePtr* th, const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>& d)>*
        ZResourcePtr_AddStatusChangedListener;

    static ThiscallEngineFunction<void(ZResourcePtr* th, const ZDelegate<void __cdecl(ZRuntimeResourceID const&)>& d)>*
        ZResourcePtr_RemoveStatusChangedListener;

    static ThiscallEngineFunction<void(ZGameLoopManager* th, const ZDelegate<void __cdecl(SGameUpdateEvent const&)>& callback, int32_t nPriority)>*
        ZGameLoopManager_RegisterForFrameUpdate;

    static ThiscallEngineFunction<void(ZGameLoopManager* th, const ZDelegate<void __cdecl(SGameUpdateEvent const&)>& callback)>*
        ZGameLoopManager_UnregisterForFrameUpdate;

    static ThiscallEngineFunction<void(ZGameLoopManager* th, EPlayMode ePlayMode)>* ZGameLoopManager_SetPlayMode;

    static ThiscallEngineFunction<ZResourcePtr*(
        ZResourceManager* th, ZResourcePtr& result, const ZRuntimeResourceID& ridResource, int32_t nPriority
    )>* ZResourceManager_GetResourcePtr;

    static ThiscallEngineFunction<ZResourcePtr*(ZResourceManager* th, ZResourcePtr& result, const ZRuntimeResourceID& ridResource)>*
        ZResourceManager_LoadResource;

    static ThiscallEngineFunction<void(ZResourceManager* th, bool bSendStatusChangedNotifications)>* ZResourceManager_Update;

    static ThiscallEngineFunction<void(ZResourceManager* th, ZResourceStub* stub)>* ZResourceManager_ReleaseStub;

    static ThiscallEngineFunction<IResourceInstaller*(const ZResourceManager* th, uint32_t nResourceType)>* ZResourceManager_GetResourceInstaller;

    static ThiscallEngineFunction<bool(ZResourceLibraryInfo* th, uint32_t nIndex, TSharedPointer<ZResourceDataBuffer> pResourceData)>*
        ZResourceLibraryInfo_InstallResource;

    static ThiscallEngineFunction<void(ZResourceLibrarySet* th, const ZDelegate<void __cdecl(void)>& d)>* ZResourceLibrarySet_RemoveReadyCallback;

    static ThiscallEngineFunction<void(ZResourceLibrarySet* th)>* ZResourceLibrarySet_Release;

    static ThiscallEngineFunction<void(
        ZDynamicResourceLibrary* th, const ZRuntimeResourceID& ridHeaderLibrary, bool bDelayedCreateEntities, uint32_t nNumInstances
    )>* ZDynamicResourceLibrary_ZDynamicResourceLibrary;

    static ThiscallEngineFunction<void(ZDynamicResourceLibrary* th)>* ZDynamicResourceLibrary_CreateEntities;

    static ThiscallEngineFunction<void(ZDynamicResourceLibrary* th)>* ZDynamicResourceLibrary_DeleteAllEntities;

    static ThiscallEngineFunction<void(ZDynamicResourceLibrary* th, const ZRuntimeResourceID& rid)>* ZDynamicResourceLibrary_OnHeaderLibraryReady;

    static ThiscallEngineFunction<void(ZDynamicResourceLibrary* th)>* ZDynamicResourceLibrary_OnResourceLibrariesReady;

    static CdeclEngineFunction<TSharedPointer<ZResourceDataBuffer>*(TSharedPointer<ZResourceDataBuffer>& result, void* data, uint32_t capacity)>*
        ZResourceDataBuffer_Create;

    static ThiscallEngineFunction<void(ZResourceLibraryLoader* th, IResourceInstaller* pInstaller, uint32_t nSize, ZRuntimeResourceID ridResource)>*
        ZResourceLibraryLoader_AllocateEntry;

    static ThiscallEngineFunction<int32_t(const ZGlobalBoneRegistry* th, char const* pBoneName)>* ZGlobalBoneRegistry_GetBoneID;

    static ThiscallEngineFunction<SMatrix*(const ZSpatialEntity* th, SMatrix& result)>* ZSpatialEntity_GetObjectToWorldMatrix;

    static ThiscallEngineFunction<void(ZSpatialEntity* th, const SMatrix& mObjectToWorld)>* ZSpatialEntity_SetObjectToWorldMatrix;

    static ThiscallEngineFunction<float4*(const ZSpatialEntity* th, float4& result)>* ZSpatialEntity_GetWorldPosition;

    static ThiscallEngineFunction<void(ZSpatialEntity* th, const float4& vWorldPosition)>* ZSpatialEntity_SetWorldPosition;

    static ThiscallEngineFunction<bool(const GFxValue* th, const char* name, GFxValue* pval)>* GFxValue_GetMember;

    static ThiscallEngineFunction<bool(GFxValue* th, const char* name, const GFxValue& val)>* GFxValue_SetMember;

    static ThiscallEngineFunction<void(ZGameWideUIScaleformHandler* th, bool bShow)>* ZGameWideUIScaleformHandler_ShowUICursor;

    static ThiscallEngineFunction<void(ZHUDManager* th, bool bShow)>* ZHUDManager_ShowHUD;

    static ThiscallEngineFunction<void(ZHUDManager* th, bool bShow)>* ZHUDManager_ShowTrespassingIcon;

    static ThiscallEngineFunction<void(ZHUDManager* th, int32_t nState)>* ZHUDManager_ShowAIStateIcon;

    static ThiscallEngineFunction<void(ZHUDManager* th, bool bShow)>* ZHUDManager_ShowThreatRadar;

    static ThiscallEngineFunction<void(ZHUDManager* th, uint32_t nElements, EHUDFadeReason eReason, bool bFadeIn, float fTime)>*
        ZHUDManager_FadeHUDElements;

    static ThiscallEngineFunction<void(ZHUDManager* th, bool bShow)>* ZHUDManager_ScaleformShowWeaponDisplay;

    static ThiscallEngineFunction<void(ZHUDManager* th)>* ZHUDManager_HideRatingTracker;

    static ThiscallEngineFunction<void(LocalResourceIDsResolver* th, const ZRuntimeResourceID& ridResource, const ZResourceID& idResource)>*
        LocalResourceIDsResolver_RecordMapping;

    static CdeclEngineFunction<ZConfigCommand*(const ZString& pszName)>* ZConfigCommand_GetConfigVariable;
};
