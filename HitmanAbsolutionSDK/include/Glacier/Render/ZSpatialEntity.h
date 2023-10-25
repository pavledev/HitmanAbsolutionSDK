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

private:
	PAD(0x38);
};

static_assert(sizeof(ZSpatialEntity) == 0x40);
static_assert(alignof(ZSpatialEntity) == 0x4);
