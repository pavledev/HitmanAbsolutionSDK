#pragma once

#include "../IComponentInterface.h"
#include "../Render/ZRenderableEntity.h"

struct SRenderViewport;
struct SVector3;
struct float4;
struct SMatrix44;
struct SVector2;
class ZEntityRef;
struct SMatrix;

class ICameraEntity : public IComponentInterface
{
public:
	enum EProjectionType
	{
		ePerspectiveRH = 0,
		eOrtogonalRH = 1,
		eCustom = 2
	};

	virtual ~ICameraEntity() = 0;
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
	virtual float4 Project(const float4& param1) const = 0;
	virtual float4 Unproject(const float4& param1) const = 0;
	virtual void CalculateBoundingRect(SVector2& vector2, SVector2& vector22, const SVector3* vector3, unsigned int param4) const = 0;
	virtual const ZEntityRef& GetPostfilter() const = 0;
	virtual const ZEntityRef& GetRootNode() const = 0;
	virtual const ZEntityRef& GetOSDRootNode() const = 0;
	virtual bool IsOrthographic() const = 0;
	virtual float4 GetOrthographicWindowSize() const = 0;
	virtual float4 GetOrthographicPixelSize() const = 0;
	virtual void LoadDefaultProjection(bool param1) = 0;
	virtual void LoadDefaultPerspectiveProjection() = 0;
	virtual void LoadDefaultOrthographicProjection(float param1) = 0;
	virtual void SetViewport(const SRenderViewport& renderViewport) = 0;
	virtual void SetDestinationViewport(const SRenderViewport& renderViewport) = 0;
	virtual void SetClearColor(SVector3 vector3) = 0;
	virtual void SetPerspectiveFovRH(float param1, float param2, float param3, float param4) = 0;
	virtual void SetOrthogonalRH(float param1, float param2, float param3, float param4) = 0;
	virtual void SetCustomProjection(const SMatrix& matrix) = 0;
	virtual void SetAutoSize(bool param1) = 0;
	virtual void SetAutoAspect(bool param1) = 0;
	virtual bool GetAutoAspect() const = 0;
	virtual void SetPostfilter(const ZEntityRef& entityRef) = 0;
	virtual void SetPostfilterOverride(bool param1, const ZEntityRef& entityRef) = 0;
	virtual void SetRootNode(const ZEntityRef& entityRef) = 0;
	virtual void SetOSDRootNode(const ZEntityRef& entityRef) = 0;
	virtual void SetOrthographicPixelSize(float param1) = 0;
	virtual void SetForceUpdateDependencies(bool param1) = 0;
	virtual bool GetForceUpdateDependencies() const = 0;
	virtual bool GetSeperateFPSFovEnabled() const = 0;
	virtual float4 GetListenerPos() const = 0;
	virtual void SetSeperateFPSFovEnabled(bool param1) = 0;
	virtual void SetAspectWByH(float param1) = 0;
	virtual void SetFPSFovYDeg(float param1) = 0;
	virtual float GetFPSFovYDeg() const = 0;
	virtual void GetGlowActive(bool& param1, bool& param2) const = 0;
	virtual void SetGlowActive(bool param1, bool param2) = 0;
	virtual bool WantCameraCut() const = 0;
	virtual void SetWantCameraCut(bool param1) = 0;
	virtual bool GetAllowAutoCameraCuts() const = 0;
	virtual float GetCameraCutTriggerDistance() const = 0;
	virtual float GetCameraCutTriggerAngleDeg() const = 0;
	virtual bool GetFPSDrawLocal() const = 0;
	virtual void SetFPSDrawLocal(bool param1) = 0;
};

class alignas(16) ZCameraEntity : public ZRenderableEntity, public ICameraEntity
{
public:
	virtual ~ZCameraEntity() = 0;
	virtual ZVariantRef GetVariantRef() const = 0;
	virtual int AddRef() = 0;
	virtual int Release() = 0;
	virtual void* QueryInterface(STypeID* iid) = 0;
	virtual void Init() = 0;
	virtual void Start() = 0;
	virtual float GetFov() const = 0;
	virtual float CalculateNearZ() const = 0;
	virtual float CalculateFarZ() const = 0;
	virtual void SetFovYDeg(float fFovYDeg) = 0;

	const SRenderViewport& GetViewport() const = 0;
	const SRenderViewport& GetDestinationViewport() const = 0;
	SVector3 GetClearColor() const = 0;
	ICameraEntity::EProjectionType GetProjectionType() const = 0;
	float GetNearZ() const = 0;
	float GetFarZ() const = 0;
	float4 GetRoomReferencePos() const = 0;
	float GetDepthExportNearZ() const = 0;
	float GetDepthExportFarZ() const = 0;
	float GetAspectWByH() const = 0;
	float GetFovYDeg() const = 0;
	const SMatrix44& GetProjectionMatrix() const = 0;
	const SMatrix44& GetFPSProjectionMatrix() const = 0;
	float4 Project(const float4& vViewPos) const = 0;
	float4 Unproject(const float4& vDevicePos) const = 0;
	void CalculateBoundingRect(SVector2& vDeviceMin, SVector2& vDeviceMax, const SVector3* pViewVertices, unsigned int nNumVertices) const = 0;
	const ZEntityRef& GetPostfilter() const = 0;
	const ZEntityRef& GetRootNode() const = 0;
	const ZEntityRef& GetOSDRootNode() const = 0;
	bool IsOrthographic() const = 0;
	float4 GetOrthographicWindowSize() const = 0;
	float4 GetOrthographicPixelSize() const = 0;
	void LoadDefaultProjection(bool bPerspective) = 0;
	void LoadDefaultPerspectiveProjection() = 0;
	void LoadDefaultOrthographicProjection(float fPixelSize) = 0;
	void SetViewport(const SRenderViewport& viewport) = 0;
	void SetDestinationViewport(const SRenderViewport& viewport) = 0;
	void SetClearColor(SVector3 vClearColor) = 0;
	void SetPerspectiveFovRH(float fFovY, float fAspectWByH, float fNear, float fFar) = 0;
	void SetOrthogonalRH(float fWidth, float fAspectWByH, float fNear, float fFar) = 0;
	void SetCustomProjection(const SMatrix& mProjection) = 0;
	void SetAutoSize(bool bAutoSize) = 0;
	void SetAutoAspect(bool bAutoAspect) = 0;
	bool GetAutoAspect() const = 0;
	void SetPostfilter(const ZEntityRef& rPostfilter) = 0;
	void SetPostfilterOverride(bool bOverridePostfilter, const ZEntityRef& rPostfilter) = 0;
	void SetRootNode(const ZEntityRef& pRootNode) = 0;
	void SetOSDRootNode(const ZEntityRef& pOSDRootNode) = 0;
	void SetOrthographicPixelSize(float fPixelSize) = 0;
	void SetForceUpdateDependencies(bool bForceUpdate) = 0;
	bool GetForceUpdateDependencies() const = 0;
	bool GetSeperateFPSFovEnabled() const = 0;
	float4 GetListenerPos() const = 0;
	void SetSeperateFPSFovEnabled(bool bEnabled) = 0;
	void SetAspectWByH(float fAspectWByH) = 0;
	void SetFPSFovYDeg(float fDegrees) = 0;
	float GetFPSFovYDeg() const = 0;
	void GetGlowActive(bool& bShowGlow, bool& bShowHints) const = 0;
	void SetGlowActive(bool bShowGlow, bool bShowHints) = 0;
	bool WantCameraCut() const = 0;
	void SetWantCameraCut(bool bCut) = 0;
	bool GetAllowAutoCameraCuts() const = 0;
	float GetCameraCutTriggerDistance() const = 0;
	float GetCameraCutTriggerAngleDeg() const = 0;
	bool GetFPSDrawLocal() const = 0;
	void SetFPSDrawLocal(bool bDrawLocal) = 0;

	SMatrix GetViewMatrix() const;

private:
	PAD(0x12C);
};

static_assert(sizeof(ZCameraEntity) == 0x190);
static_assert(alignof(ZCameraEntity) == 0x10);
