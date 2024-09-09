#pragma once

#include "../IComponentInterface.h"
#include "../Render/ZRenderableEntity.h"
#include "../Render/IRenderPostfilterControllerEntity.h"

struct SRenderViewport;
struct SVector3;
struct float4;
struct SMatrix44;
struct SVector2;
class ZEntityRef;
struct SMatrix;

class HitmanAbsolutionSDK_API ICameraEntity : public IComponentInterface
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
	virtual void CalculateBoundingRect(SVector2& vDeviceMin, SVector2& vDeviceMax, const SVector3* pViewVertices, unsigned int nNumVertices) const = 0;
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

class HitmanAbsolutionSDK_API alignas(16) ZCameraEntity : public ZRenderableEntity, public ICameraEntity
{
public:
	virtual float GetFov() const = 0;
	virtual float CalculateNearZ() const = 0;
	virtual float CalculateFarZ() const = 0;
	virtual void SetFovYDeg(float fFovYDeg) = 0;

	SMatrix GetViewMatrix() const;
	void SetNearZ(float fNearZ);
	void SetFarZ(float fFarZ);
	TEntityRef<IRenderPostfilterControllerEntity>& GetRenderPostfilterControllerEntity();

private:
	PAD(0x40);
	TEntityRef<IRenderPostfilterControllerEntity> m_rPostfilter; //0xA4
	PAD(0xDC);
};

static_assert(sizeof(ZCameraEntity) == 0x190);
static_assert(alignof(ZCameraEntity) == 0x10);
