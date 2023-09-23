#pragma once

#include "Entity/ZEntityImpl.h"
#include "Templates/TEntityRef.h"
#include "Utility/Alignment.h"

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

class IHM5BaseCharacter : public IComponentInterface
{
public:
	virtual ~IHM5BaseCharacter() = 0;
};

class IBaseCharacter : public IComponentInterface
{
public:
	virtual ~IBaseCharacter() = 0;
	virtual void YouGotHit(const SHitInfo& hitInfo) = 0;
	virtual bool CanProjectileHitCharacter(const SHitInfo& hitInfo) = 0;
	virtual unsigned short GetCollisionLayer() const = 0;
	virtual bool RegisterAttachment(unsigned int param1, TEntityRef<ZSpatialEntity> entityRef) = 0;
	virtual void UnregisterAttachment(unsigned int param1, TEntityRef<ZSpatialEntity> entityRef) = 0;
	virtual bool IsRagdoll() const = 0;
	virtual ZEntityRef GetLinkedEntityBase() const = 0;
};

class IMorphemeCutSequenceAnimatable
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
	virtual ~IMorphemeCutSequenceAnimatable() = 0;
};

class IBoneCollidable
{
public:
	virtual bool CheckLineCollision(const ZRayQueryInput& rayQueryInput, ZRayQueryOutput* rayQueryOutput) const = 0;
	virtual ICharacterController* GetCharacterController() const = 0;
	virtual ~IBoneCollidable() = 0;
};

class alignas(16) ZHM5BaseCharacter : public ZEntityImpl, public IHM5BaseCharacter, public IBaseCharacter, public IMorphemeCutSequenceAnimatable, public IBoneCollidable
{
public:
	virtual ~ZHM5BaseCharacter() = 0;
	virtual ZVariantRef GetVariantRef() const = 0;
	virtual int AddRef() = 0;
	virtual int Release() = 0;
	virtual void* QueryInterface(STypeID* iid) = 0;
	virtual void Activate(const ZString& sSubset) = 0;
	virtual void Deactivate(const ZString& sSubset) = 0;
	virtual void Init() = 0;
	virtual bool PickupItem(const TEntityRef<IHM5Item>& rItem) = 0;
	virtual bool SwapItem(const TEntityRef<IHM5Item>& rNewItem, const TEntityRef<IHM5Item>& rDisposedItem) = 0;
	virtual bool AddItemToInventory(const TEntityRef<IHM5Item>& rItem) = 0;
	virtual void RemoveItemFromInventory(const TEntityRef<IHM5Item>& rItem) = 0;
	virtual bool IsHandsEmpty() const = 0;
	virtual bool IsHoldingWeapon() const = 0;
	virtual void AttachItemToRHand(const TEntityRef<IHM5Item>& rItem) = 0;
	virtual void AttachItemToLHand(const TEntityRef<IHM5Item>& rItem) = 0;
	virtual void AttachItemToFreeBone(const TEntityRef<IHM5Item>& rItem) = 0;
	virtual TEntityRef<IHM5Item> DetachItemFromRHand(bool bAddToPhysicWorld) = 0;
	virtual TEntityRef<IHM5Item> DetachItemFromLHand(bool bAddToPhysicWorld) = 0;
	virtual TEntityRef<IHM5Item> DetachItemFromFreeBone(bool bAddToPhysicWorld) = 0;
	virtual void ItemDestoyed(TEntityRef<IHM5Item> pItem) = 0;
	virtual void SetRagdollSpeedMultiplier(float f) = 0;
	virtual bool IsArmed() = 0;
	virtual void AimCallBack(bool bRightHand) = 0;
	virtual TEntityRef<ZGeomEntity> GetGeomEntity() const = 0;
	virtual ZGeomEntity* GetGeomEntityPtr() const = 0;
	virtual ZSpatialEntity* GetSpatialEntityPtr() const = 0;
	virtual TEntityRef<ZSpatialEntity> GetSpatialEntity() const = 0;
	virtual ZLinkedEntity* GetLinkedEntityPtr() const = 0;
	virtual TEntityRef<ZLinkedEntity> GetLinkedEntity() const = 0;
	virtual bool DeactivateLocoMotion() = 0;
	virtual void ShowCharacter(bool bShow) = 0;
	virtual bool IsCharacterHidden() const = 0;
	virtual bool IsPoweredRagdoll() const = 0;
	virtual bool IsPoweredRagdollStopping() const = 0;
	virtual bool IsRagdollGrabbed() const = 0;
	virtual bool IsRagdollControlled() const = 0;
	virtual IPhysicsObject* GetGrabAttacher() const = 0;
	virtual bool InValidGrabRagdollState() const = 0;
	virtual bool CanGrabRagdoll(unsigned int& nPartId, const float4& vPos, float fThreshold) const = 0;
	virtual bool GrabRagdoll(unsigned int nPartId, const float4& vPos) = 0;
	virtual void UpdateGrabPosition(const SMatrix& mTrans) = 0;
	virtual void ReleaseGrabbedRagdoll(ICharacterController* pGrabber) = 0;
	virtual void ActivateRagdoll() = 0;
	virtual void ActivatePoweredRagdoll(bool bImpactPower, bool bHeadshot) = 0;
	virtual void DeactivatePoweredRagdoll(float fBlendOutTime, bool bToAnimation) = 0;
	virtual void ReleaseRagdoll() = 0;
	virtual void WakeUpRagdoll(float fTime) = 0;
	virtual void SetUpdateRagdollOnce() = 0;
	virtual void ActivateRagdollInMotion() = 0;
	virtual bool IsPoweredRagdollActive() const = 0;
	virtual bool RequestAnimationDriven() = 0;
	virtual bool IsWearingOutfit() const = 0;
	virtual const TEntityRef<IHM5Outfit> GetOutfit() const = 0;
	virtual void SetOriginalOutfit(TEntityRef<IHM5Outfit> pOutfit) = 0;
	virtual const TEntityRef<IHM5Outfit> GetOriginalOutfit() const = 0;
	virtual void SetOutfit(const TEntityRef<IHM5Outfit>& outfit) = 0;
	virtual bool WillRequestBeReceived(const unsigned int nRequestID) const = 0;
	virtual bool WillRequestBeReceived(const ZString& sRequest) const = 0;
	virtual bool IsDead() const = 0;
	virtual bool CanBeDragged() const = 0;
	virtual TEntityRef<IMorphemeEntity> GetMorphemeEntity() const = 0;
	virtual bool CanPlayAnim(const SMatrix& mStart, unsigned int iAnimNode) = 0;
	virtual bool CanPlayAnim(const SMatrix& mStart, unsigned int iAnimNode, unsigned int* apBoneList, unsigned int iNumBones, float* apFractions, unsigned int iNumFractions, float* pfFractionFailed) = 0;
	virtual bool IsOnPathFinder(unsigned int* apBoneList, unsigned int iNumBones, float fHeightOffset) const = 0;
	virtual void SampleAnim(unsigned int iAnimNode, unsigned int* apBoneList, unsigned int iNumBones, float* apFractions, unsigned int iNumFractions, SAnimSample& AnimSample) = 0;
	virtual void ResetCharacter() = 0;
	virtual SMatrix GetObjectToWorldMatrix() const = 0;
	virtual const SMatrix GetObjectToParentMatrix() const = 0;
	virtual float4 GetWorldPosition() const = 0;
	virtual float4 GetWorldForward() const = 0;
	virtual const float4 GetLocalPosition() const = 0;
	virtual void GetCharacterOBB(SMatrix& m0, float4& vSize) const = 0;
	virtual ZEvent<TEntityRef<IHM5Outfit>, TEntityRef<IHM5Outfit>, ZEventNull, ZEventNull, ZEventNull>& GetOutfitChangedEvent() = 0;
	virtual void SetInstantiateNetwork(bool bInstantiate) = 0;
	virtual void ReinitializeMorphemeData() = 0;
	virtual void AddEventConsumer(TEntityRef<IEventConsumerCollection> pConsumer) = 0;
	virtual void RemoveEventConsumer(TEntityRef<IEventConsumerCollection> pConsumer) = 0;
	virtual void GetOBB(SMatrix& mvCen, float4& vHalfSize) const = 0;
	virtual float4 GetOBBRadius() const = 0;
	virtual SMatrix GetCenterMatPos() const = 0;
	virtual bool IsInitializationDone() const = 0;
	virtual bool CreateBodyCollision() = 0;
	virtual void UpdateBodyCollisionPoses(bool bMoveTransform) = 0;
	virtual void OnAnimatorChanged() = 0;
	virtual bool ShouldDoHardCrowdPushes() const = 0;
	virtual void SetSequenceWeight(float weight, unsigned int track) = 0;
	virtual void SetSequenceFeatherWeight(float weight, unsigned int track) = 0;
	virtual void BlendOutSequenceWeights() = 0;
	virtual void OnOutfitEntityChanged() = 0;
	virtual void HandleMorphemeEntityIDChanged() = 0;
	virtual ZHM5Ragdoll* AcquireRagdollInstance(const float4& vSearchPos) = 0;
	virtual bool EnsureBaseCharacterInit() = 0;

	virtual void YouGotHit(const SHitInfo& HitInfo) = 0;
	virtual bool CanProjectileHitCharacter(const SHitInfo& HitInfo) = 0;
	virtual unsigned short GetCollisionLayer() const = 0;
	virtual bool RegisterAttachment(unsigned int nBoneID, TEntityRef<ZSpatialEntity> Attachment) = 0;
	virtual void UnregisterAttachment(unsigned int nBoneID, TEntityRef<ZSpatialEntity> Attachment) = 0;
	virtual bool IsRagdoll() const = 0;
	virtual ZEntityRef GetLinkedEntityBase() const = 0;

	virtual void CutSequenceStarted(ISequenceEntity* pSequence, const ZString& sSlotName, float fBodyPartSelector) = 0;
	virtual void CutSequenceUpdate(ISequenceEntity* pSequence, ZString sSlotName, float fWeight) = 0;
	virtual SMatrix43 CutSequenceGetWorldTransform() = 0;
	virtual void CutSequenceSetWorldTransform(const SMatrix43& mNewWorldTransform) = 0;
	virtual SMatrix43 CutSequenceGetParentTransform() = 0;
	virtual void CutSequenceSetParentTransform(const SMatrix43& mNewParentTransform) = 0;
	virtual void CutSequenceEnded(ISequenceEntity* pSequence, ZString sSlotName) = 0;
	virtual ZEvent0& GetAnimatableChangedEvent() = 0;
	virtual void AbortCutSequence() = 0;
	virtual bool SetCutSequenceData(ISequenceEntity* pSequence, const SCutSequenceData& cutSequenceData, const ZString& sSlotName, float fBodyPartSelector) = 0;

	virtual bool CheckLineCollision(const ZRayQueryInput& sInput, ZRayQueryOutput* pOutput) const = 0;
	virtual ICharacterController* GetCharacterController() const = 0;

private:
	PAD(0x1D8);
};

static_assert(sizeof(ZHM5BaseCharacter) == 0x1F0);
static_assert(alignof(ZHM5BaseCharacter) == 0x10);
