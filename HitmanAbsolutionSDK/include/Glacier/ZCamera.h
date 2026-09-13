#pragma once

#include <cstdint>

#include "IComponentInterface.h"
#include "SColorRGB.h"
#include "ZRender.h"

#undef max

struct SRenderViewport;
struct SVector3;
struct float4;
struct SMatrix44;
struct SVector2;
class ZEntityRef;
struct SMatrix;

struct SRenderViewport
{
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
};

class ICameraEntity : public IComponentInterface
{
  public:
    enum EProjectionType
    {
        ePerspectiveRH = 0,
        eOrtogonalRH = 1,
        eCustom = 2
    };

    virtual const SRenderViewport& GetViewport() const = 0;
    virtual const SRenderViewport& GetDestinationViewport() const = 0;
    virtual SVector3 GetClearColor() const = 0;
    virtual EProjectionType GetProjectionType() const = 0;
    virtual float GetNearZ() const = 0;
    virtual float GetFarZ() const = 0;
    virtual float4 GetRoomReferencePos() const = 0;
    virtual float GetDepthExportNearZ() const = 0;
    virtual float GetDepthExportFarZ() const = 0;
    virtual float GetAspectWByH() const = 0;
    virtual float GetFovYDeg() const = 0;
    virtual const SMatrix44& GetProjectionMatrix() const = 0;
    virtual const SMatrix44& GetFPSProjectionMatrix() const = 0;
    virtual float4 Project(const float4& vViewPos) const = 0;
    virtual float4 Unproject(const float4& vDevicePos) const = 0;
    virtual void CalculateBoundingRect(SVector2& vDeviceMin, SVector2& vDeviceMax, const SVector3* pViewVertices, uint32_t nNumVertices) const = 0;
    virtual const ZEntityRef& GetPostfilter() const = 0;
    virtual const ZEntityRef& GetRootNode() const = 0;
    virtual const ZEntityRef& GetOSDRootNode() const = 0;
    virtual bool IsOrthographic() const = 0;
    virtual float4 GetOrthographicWindowSize() const = 0;
    virtual float4 GetOrthographicPixelSize() const = 0;
    virtual void LoadDefaultProjection(bool bPerspective) = 0;
    virtual void LoadDefaultPerspectiveProjection() = 0;
    virtual void LoadDefaultOrthographicProjection(float fPixelSize) = 0;
    virtual void SetViewport(const SRenderViewport& viewport) = 0;
    virtual void SetDestinationViewport(const SRenderViewport& viewport) = 0;
    virtual void SetClearColor(SVector3 vClearColor) = 0;
    virtual void SetPerspectiveFovRH(float fFovY, float fAspectWByH, float fNear, float fFar) = 0;
    virtual void SetOrthogonalRH(float fWidth, float fAspectWByH, float fNear, float fFar) = 0;
    virtual void SetCustomProjection(const SMatrix& mProjection) = 0;
    virtual void SetAutoSize(bool bAutoSize) = 0;
    virtual void SetAutoAspect(bool bAutoAspect) = 0;
    virtual bool GetAutoAspect() const = 0;
    virtual void SetPostfilter(const ZEntityRef& rPostfilter) = 0;
    virtual void SetPostfilterOverride(bool bOverridePostfilter, const ZEntityRef& rPostfilter) = 0;
    virtual void SetRootNode(const ZEntityRef& pRootNode) = 0;
    virtual void SetOSDRootNode(const ZEntityRef& pOSDRootNode) = 0;
    virtual void SetOrthographicPixelSize(float fPixelSize) = 0;
    virtual void SetForceUpdateDependencies(bool bForceUpdate) = 0;
    virtual bool GetForceUpdateDependencies() const = 0;
    virtual bool GetSeperateFPSFovEnabled() const = 0;
    virtual float4 GetListenerPos() const = 0;
    virtual void SetSeperateFPSFovEnabled(bool bEnabled) = 0;
    virtual void SetAspectWByH(float fAspectWByH) = 0;
    virtual void SetFPSFovYDeg(float fDegrees) = 0;
    virtual float GetFPSFovYDeg() const = 0;
    virtual void GetGlowActive(bool& bShowGlow, bool& bShowHints) const = 0;
    virtual void SetGlowActive(bool bShowGlow, bool bShowHints) = 0;
    virtual bool WantCameraCut() const = 0;
    virtual void SetWantCameraCut(bool bCut) = 0;
    virtual bool GetAllowAutoCameraCuts() const = 0;
    virtual float GetCameraCutTriggerDistance() const = 0;
    virtual float GetCameraCutTriggerAngleDeg() const = 0;
    virtual bool GetFPSDrawLocal() const = 0;
    virtual void SetFPSDrawLocal(bool bDrawLocal) = 0;
};

class alignas(16) ZCameraEntity : public ZRenderableEntity, public ICameraEntity
{
  public:
    virtual float GetFov() const = 0;

    SMatrix GetViewMatrix() const
    {
        return GetObjectToWorldMatrix().Inverse();
    }

    void SetFovYDeg(float p_FOVYDeg)
    {
        if (m_fFovYDeg != p_FOVYDeg)
        {
            constexpr float degToRad = 3.1415927f / 180.0f;
            const float fFovYRad = p_FOVYDeg * degToRad;

            m_fFovYDeg = p_FOVYDeg;
            m_fFovY = std::max(fFovYRad, 0.01f);

            Functions::ZCameraEntity_UpdateProjection->Call(this);
        }
    }

    void SetNearZ(float p_NearZ)
    {
        Functions::ZCameraEntity_SetNearZ->Call(this, p_NearZ);
    }

    void SetFarZ(float p_FarZ)
    {
        Functions::ZCameraEntity_SetFarZ->Call(this, p_FarZ);
    }

    void UpdateProjection()
    {
        Functions::ZCameraEntity_UpdateProjection->Call(this);
    }

    EProjectionType m_eProjectionType;
    float m_fFovYDeg;
    float m_fFPSFovYDeg;
    float m_fWidth;
    float m_fAspectWByH;
    float m_fNearZ;
    float m_fFarZ;
    ZEntityRef m_pRootNode;
    ZEntityRef m_pOSDRootNode;
    SColorRGB m_vClearColor;
    bool m_bAutoSize;
    bool m_bAutoAspect;
    bool m_bFPSSeperateFOV;
    bool m_bAllowAutoCameraCuts;
    float m_fCameraCutTriggerDistance;
    float m_fCameraCutTriggerAngleDeg;
    ZResourcePtr m_pHelper;
    TEntityRef<IRenderPostfilterControllerEntity> m_rPostfilter;
    TEntityRef<ZSpatialEntity> m_pRoomReferencePosOverride;
    float4 m_vRoomReferencePosOverrideCode;
    float m_fDepthExportNearZ;
    float m_fDepthExportFarZ;
    float m_fFovY;
    float m_fFPSFovY;
    SMatrix44 m_mProjection;
    SMatrix44 m_mFPSProjection;
    SRenderViewport m_Viewport;
    SRenderViewport m_DestinationViewport;
    float m_fAutoAspectWByH;
    TEntityRef<IRenderPostfilterControllerEntity> m_rPostfilterOverride;
    bool m_bOverridePostfilter : 1;
    bool m_bForceUpdated : 1;
    bool m_bGlowActive : 1;
    bool m_bHintsActive : 1;
    bool m_bWantCameraCut : 1;
    bool m_bFPSDrawLocal : 1;
    bool m_bRoomReferencePosOverrideCode : 1;
};

static_assert(sizeof(ZCameraEntity) == 0x190);
static_assert(alignof(ZCameraEntity) == 0x10);

class ZFreeCameraControlEntity : public ZEntityImpl
{
  public:
    void SetActive(bool p_Active)
    {
        Functions::ZFreeCameraControlEntity_SetActive->Call(this, p_Active);
    }

    float4 GetUpdatedCameraPosition(float p_MoveX, float p_MoveY, float p_MoveZ, const SMatrix& p_CurrentCameraToWorld)
    {
        float4 result;
        Functions::ZFreeCameraControlEntity_GetUpdatedCameraPosition->Call(this, result, p_MoveX, p_MoveY, p_MoveZ, p_CurrentCameraToWorld);

        return result;
    }

    SMatrix GetUpdatedCameraRotation(float p_DeltaRoll, float p_DeltaPitch, float p_DeltaYaw, const SMatrix& p_CurrentCameraToWorld)
    {
        SMatrix result;
        Functions::ZFreeCameraControlEntity_GetUpdatedCameraRotation->Call(
            this, result, p_DeltaRoll, p_DeltaPitch, p_DeltaYaw, p_CurrentCameraToWorld
        );

        return result;
    }

    ZEntityRef m_cameraEntity;
    bool m_bActive;
    bool m_bIsGameControlActive;
    ZCameraEntity* m_pControlledCameraEntity;
    float m_fMoveSpeed;
    float m_fTurnSpeed;
    float m_fMovementSpeedMultiplier;
    float m_fRotationSpeedMultiplier;
    float m_fMoveX;
    float m_fMoveY;
    float m_fMoveZ;
    bool m_bMoveInWorldSpace;
    bool m_bResetRoll;
    float m_fPitch;
    float m_fDeltaPitch;
    float m_fRoll;
    float m_fDeltaRoll;
    float m_fDeltaYaw;
    float m_fInitialFov;
    float m_fFov;
    float m_fDeltaFov;
    int32_t m_nControllerId;
};

class ZHM5MainCamera : public ZCameraEntity
{
  public:
    void SetCameraDirection(const float4& p_LookTo)
    {
        Functions::ZHM5MainCamera_SetCameraDirection->Call(this, p_LookTo);
    }

    PAD(0x3C0);
};

static_assert(sizeof(ZHM5MainCamera) == 0x550);
static_assert(alignof(ZHM5MainCamera) == 0x10);
