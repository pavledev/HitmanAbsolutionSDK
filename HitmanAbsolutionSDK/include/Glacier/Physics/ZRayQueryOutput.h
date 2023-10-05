#pragma once

#include "ZRayQueryOutputBase.h"
#include "../Entity/ZEntityRef.h"
#include "ZPhysicsObjectRef.h"
#include "../Templates/TEntityRef.h"
#include "SBoneCollision.h"

class ZSpatialEntity;
class ZRenderPrimitiveResource;

class HitmanAbsolutionSDK_API ZRayQueryOutput : public ZRayQueryOutputBase
{
public:
	const ZEntityRef& GetBlockingEntity() const;

private:
	ZEntityRef m_BlockingEntity;
	ZPhysicsObjectRef m_pBlockingPhysicsObject;
	TEntityRef<ZSpatialEntity> m_pBlockingSpatialEntity;
	TResourcePtr<ZRenderPrimitiveResource> m_pBlockingPrimitive;
	float4 m_vDecalPosition;
	float4 m_vDecalNormal;
	TArray<SBoneCollision> m_aAllIntersectedBones;
};
