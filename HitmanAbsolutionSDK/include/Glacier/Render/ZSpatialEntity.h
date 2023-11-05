#pragma once

#include "../Entity/ZEntityImpl.h"
#include "../Templates/TEntityRef.h"
#include "../Math/SMatrix.h"

#include <Common.h>

class ZBoundedEntity;
struct float4;

class HitmanAbsolutionSDK_API ZSpatialEntity : public ZEntityImpl
{
public:
	enum ERoomBehaviour
	{
		ROOM_STATIC = 0,
		ROOM_DYNAMIC = 1
	};

	virtual void SetVisible(bool bVisible) = 0;
	virtual const ZBoundedEntity* QueryBoundedEntity() const = 0;
	virtual ZBoundedEntity* QueryBoundedEntity() = 0;
	virtual void SetPrivate(bool bPrivate) = 0;
	virtual bool IsPrivate() const = 0;
	virtual void SetFPSDrawMode(bool bFPSDrawMode) = 0;
	virtual void SetEditorSelected(bool bSelected) = 0;
	virtual bool GetEditorSelected() const = 0;
	virtual void SetEditorVisible(bool bVisible) = 0;
	virtual bool GetEditorVisible() const = 0;
	virtual void SetTransformParent(TEntityRef<ZSpatialEntity> Parent) = 0;
	virtual TEntityRef<ZSpatialEntity> GetTransformParentEntityPtr() const = 0;
	virtual ZSpatialEntity* GetScene() const = 0;
	virtual void CalculateBounds(float4& vMin_, float4& vMax_, const unsigned int nIncludeFlags, const unsigned int nExcludeFlags) = 0;
	virtual float4 GetLocalCenter() const = 0;
	virtual float4 GetLocalHalfSize() const = 0;
	virtual void OnObjectToParentChanged() = 0;
	virtual void OnTransformParentIDChanged() = 0;
	virtual void OnVisibleChanged(const bool& bWasVisible) = 0;
	virtual void OnSpatialChange() = 0;

	SMatrix GetObjectToWorldMatrix() const;
	void SetObjectToWorldMatrix(const SMatrix& mObjectToWorld);
	float4 GetWorldPosition() const;
	void SetWorldPosition(const float4& vWorldPosition);
	SMatrix GetViewMatrix() const;
	ZSpatialEntity* GetTransformParent() const;

private:
	ZSpatialEntity* m_pTransformParent;
	ZSpatialEntity* m_pTransformChildren;
	ZSpatialEntity* m_pTransformNext;
	ZSpatialEntity* m_pTransformPrev;
	ZEntityRef m_transformParentID;
	SVector4 m_vObjectToParentRotation;
	SVector3 m_vObjectToParentTranslation;
	bool m_bVisible;
	bool m_bIsPrivate;
	bool m_bDisplayBounds : 1;
	bool m_bDisplayPivot : 1;
	bool m_bDisplayHelpers : 1;
	bool m_bDisplayHelperPrimitive : 1;
	bool m_bEditorSelected : 1;
	bool m_bEditorVisible : 1;
	bool m_bParentHidden : 1;
	bool m_bSceneChild : 1;
	bool m_bFPSDrawMode : 1;
	bool m_bNotifyChange : 1;
	bool m_bRoomDynamicAlways : 1;
	bool m_bRoomDynamic : 1;
	bool m_bParentDynamic : 1;
	bool m_bIsDynamic : 1;
	bool m_bIncludeInParentsBounds : 1;
	bool m_bUpdateBoundsPending : 1;
	ZEvent<ZEntityRef const&, SMatrix43 const&, ZEventNull, ZEventNull, ZEventNull>* m_pTransformChangeCallBackEvent;
};

static_assert(sizeof(ZSpatialEntity) == 0x40);
static_assert(alignof(ZSpatialEntity) == 0x4);
