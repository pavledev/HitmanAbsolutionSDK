#pragma once

#include "Entity/ZEntityImpl.h"
#include "Templates/TEntityRef.h"

#include <Common.h>

struct SHitInfo;
class ZSpatialEntity;
class ISequenceEntity;
struct SMatrix43;
class ZEvent0;
struct SCutSequenceData;
class ZRayQueryInput;
class ZRayQueryOutput;
class ICharacterController;
class IHM5Item;
class ZGeomEntity;
class ZLinkedEntity;
class IHM5Outfit;
class IPhysicsObject;
class IMorphemeEntity;
struct SAnimSample;
class IEventConsumerCollection;
class ZHM5Ragdoll;
struct float4;
struct SMatrix;

class HitmanAbsolutionSDK_API IHM5BaseCharacter : public IComponentInterface
{
public:
	virtual ~IHM5BaseCharacter() = default;
};

class HitmanAbsolutionSDK_API IBaseCharacter : public IComponentInterface
{
public:
	virtual ~IBaseCharacter() = default;
	virtual void YouGotHit(const SHitInfo& hitInfo) = 0;
	virtual bool CanProjectileHitCharacter(const SHitInfo& hitInfo) = 0;
	virtual unsigned short GetCollisionLayer() const = 0;
	virtual bool RegisterAttachment(unsigned int param1, TEntityRef<ZSpatialEntity> entityRef) = 0;
	virtual void UnregisterAttachment(unsigned int param1, TEntityRef<ZSpatialEntity> entityRef) = 0;
	virtual bool IsRagdoll() const = 0;
	virtual ZEntityRef GetLinkedEntityBase() const = 0;
};

class HitmanAbsolutionSDK_API IMorphemeCutSequenceAnimatable
{
public:
	virtual void CutSequenceStarted(ISequenceEntity* sequenceEntity, const ZString& string, float param3) = 0;
	virtual void CutSequenceUpdate(ISequenceEntity* sequenceEntity, ZString string, float param3) = 0;
	virtual SMatrix43 CutSequenceGetWorldTransform() = 0;
	virtual void CutSequenceSetWorldTransform(const SMatrix43& matrix43) = 0;
	virtual SMatrix43 CutSequenceGetParentTransform() = 0;
	virtual void CutSequenceSetParentTransform(const SMatrix43& matrix43) = 0;
	virtual void CutSequenceEnded(ISequenceEntity* sequenceEntity, ZString string) = 0;
	virtual ZEvent0& GetAnimatableChangedEvent() = 0;
	virtual void AbortCutSequence() = 0;
	virtual bool SetCutSequenceData(ISequenceEntity* sequenceEntity, const SCutSequenceData& cutSequenceData, const ZString& string, float param4) = 0;
	virtual ~IMorphemeCutSequenceAnimatable() = default;
};

class HitmanAbsolutionSDK_API IBoneCollidable
{
public:
	virtual bool CheckLineCollision(const ZRayQueryInput& rayQueryInput, ZRayQueryOutput* rayQueryOutput) const = 0;
	virtual ICharacterController* GetCharacterController() const = 0;
	virtual ~IBoneCollidable() = default;
};

class HitmanAbsolutionSDK_API alignas(16) ZHM5BaseCharacter : public ZEntityImpl, public IHM5BaseCharacter, public IBaseCharacter, public IMorphemeCutSequenceAnimatable, public IBoneCollidable
{
private:
	PAD(0x1D8);
};

static_assert(sizeof(ZHM5BaseCharacter) == 0x1F0);
static_assert(alignof(ZHM5BaseCharacter) == 0x10);
