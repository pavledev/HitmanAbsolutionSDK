#pragma once

#include "IComponentInterface.h"
#include "ZMath.h"
#include "ZEntity.h"
#include "ZResource.h"
#include "ZPresenceState.h"
#include "ZEvent.h"

enum EPhysicsObjectType;
enum ECollisionPriority;
enum ECCDUsage;
enum ECOMUsage;
enum ECameraCollisionMode;
class ZPhysicsObjectProxy;
class ICollisionShape;
struct SCollisionInfo;
struct SHitInfo;
struct SCapsule;
class ZSpatialEntity;
class NxShape;
class NxContactPair;
class ZRenderMaterialInstance;
class ZRenderPrimitiveResource;

enum ECollidablesType
{
    ECST_STATIC_AND_DYNAMIC = 0,
    ECST_STATIC_ONLY = 1,
    ECST_DYNAMIC_ONLY = 2
};

enum ERayDetailLevel
{
    RAYDETAILS_NONE = 0,
    RAYDETAILS_BONES = 1,
    RAYDETAILS_MESH = 2
};

class ICharacterCollision : public IComponentInterface
{
  public:
    virtual ~ICharacterCollision() = default;
    virtual SMatrix GetCollisionCheckedMatPos(const SMatrix& matrix) = 0;
};

class ICollisionManager : public IComponentInterface
{};

class ICollisionShapeListener : public IComponentInterface
{
  public:
    virtual ~ICollisionShapeListener() = 0;
    virtual void OnCollisionShapeChanged() = 0;
};

class IPhysicsObject
{
  public:
    virtual ~IPhysicsObject() = default;
    virtual SMatrix GetTransform() const = 0;
    virtual SMatrix GetTransformInterpolated() const = 0;
    virtual SMatrix GetPreviousTransform() const = 0;
    virtual void SetTransform(const SMatrix& matrix) = 0;
    virtual void MoveTransform(const SMatrix& matrix) = 0;
    virtual float4 GetPosition() const = 0;
    virtual float4 GetPositionInterpolated() const = 0;
    virtual void SetPosition(const float4& param1) = 0;
    virtual void MovePosition(const float4& param1) = 0;
    virtual void RestoreWorldInteraction() = 0;
    virtual void DisableWorldInteraction() = 0;
    virtual bool IsWorldInteractionEnabled() = 0;
    virtual void SetType(EPhysicsObjectType physicsObjectType) = 0;
    virtual EPhysicsObjectType GetType() const = 0;
    virtual void SetPriority(ECollisionPriority collisionPriority) = 0;
    virtual ECollisionPriority GetPriority() const = 0;
    virtual bool SetContinuousCollisionDetection(const ECCDUsage usage) = 0;
    virtual ECCDUsage GetContinuousCollisionDetection() const = 0;
    virtual void SetCenterOfMassUsage(const ECOMUsage usage) = 0;
    virtual ECOMUsage GetCenterOfMassUsage() const = 0;
    virtual void SetSkinWidth(const float param1) = 0;
    virtual float GetSkinWidth() const = 0;
    virtual void SetCameraCollisionMode(const ECameraCollisionMode cameraCollisionMode) = 0;
    virtual ECameraCollisionMode GetCameraCollisionMode() const = 0;
    virtual float GetMass() const = 0;
    virtual void SetMass(float param1) = 0;
    virtual float4 GetCenterOfMass() const = 0;
    virtual float4 GetCenterOfMassLocal() const = 0;
    virtual SMatrix GetInertiaTensor() const = 0;
    virtual void AffectedByGravity(const bool param1) = 0;
    virtual float GetLinearDamping() const = 0;
    virtual void SetLinearDamping(float param1) = 0;
    virtual float GetAngularDamping() const = 0;
    virtual void SetAngularDamping(float param1) = 0;
    virtual float GetFriction() const = 0;
    virtual void SetFriction(float param1) = 0;
    virtual float GetRestitution() const = 0;
    virtual void SetRestitution(float param1) = 0;
    virtual void SetEntity(const ZEntityRef& entityRef) = 0;
    virtual ZEntityRef GetEntity() const = 0;
    virtual ICollisionShape* GetCollisionShape() const = 0;
    virtual bool IsKinematic() const = 0;
    virtual void SetKinematic(bool param1) = 0;
    virtual bool IsDynamic() const = 0;
    virtual bool IsSleeping() const = 0;
    virtual bool WakeUp(float param1) = 0;
    virtual bool PutToSleep() = 0;
    virtual void ResetMotion() = 0;
    virtual void SetSleepEnergyThreshold(float param1) = 0;
    virtual void ApplyLinearVelocity(const float4& param1) = 0;
    virtual void ApplyLinearImpulse(const float4& param1) = 0;
    virtual void ApplyForce(const float4& param1) = 0;
    virtual void ApplyPointImpulse(const float4& param1, const float4& param2) = 0;
    virtual void ApplyPointForce(const float4& param1, const float4& param2) = 0;
    virtual void ApplyPointImpulseLocalPos(const float4& param1, const float4& param2) = 0;
    virtual void ApplyPointForceLocalPos(const float4& param1, const float4& param2) = 0;
    virtual void ApplyTorqueImpulse(const float4& param1) = 0;
    virtual void ApplyTorque(const float4& param1) = 0;
    virtual void ApplyLocalLinearVelocity(const float4& param1) = 0;
    virtual void ApplyLocalLinearImpulse(const float4& param1) = 0;
    virtual void ApplyLocalForce(const float4& param1) = 0;
    virtual void ApplyLocalPointImpulse(const float4& param1, const float4& param2) = 0;
    virtual void ApplyLocalPointForce(const float4& param1, const float4& param2) = 0;
    virtual void ApplyLocalTorqueImpulse(const float4& param1) = 0;
    virtual void ApplyLocalTorque(const float4& param1) = 0;
    virtual float GetKineticEnergy() const = 0;
    virtual float4 GetLinearVelocity() const = 0;
    virtual float4 GetAngularVelocity() const = 0;
    virtual void SetLinearVelocity(const float4& param1) = 0;
    virtual void SetLinearVelocityLocal(const float4& param1) = 0;
    virtual void SetAngularVelocity(const float4& param1) = 0;
    virtual void SetMaxAngularVelocity(const float param1) = 0;
    virtual bool IsObjectInPhysicsWorld() const = 0;
    virtual void SetCollisionActivity(bool param1, bool param2) = 0;
    virtual bool GetCollisionActivity(bool param1) const = 0;
    virtual void SetCollisionLayer(const uint16_t param1) = 0;
    virtual uint16_t GetCollisionLayer() const = 0;
    virtual void SetCollisionGroup(const uint16_t param1) = 0;
    virtual uint16_t GetCollisionGroup() const = 0;
    virtual void UpdateStaticCollisionLayers() = 0;
    virtual SCapsule GetG2Capsule() const = 0;
    virtual void ConditionalAdjustCapsule(const float4& param1, const float4& param2, const float param3, const float param4) = 0;
    virtual void SetGenerateSound(const bool param1) = 0;
    virtual bool GetGenerateSound() const = 0;
    virtual void SetGenerateSoundLoud(const bool param1) = 0;
    virtual bool GetGenerateSoundLoud() const = 0;
    virtual void SetIsRagdollPart(const bool param1) = 0;
    virtual bool GetIsRagdollPart() const = 0;
    virtual void SetIgnoringCharacters(const bool param1) = 0;
    virtual void OnCollision(const SCollisionInfo& collisionInfo) = 0;
    virtual void OnHit(const SHitInfo& hitInfo) = 0;
    virtual float GetCombinedRestitution(const float param1) const = 0;
    virtual ZPhysicsObjectProxy* GetProxy() const = 0;
    virtual void SetProxy(ZPhysicsObjectProxy* physicsObjectProxy) const = 0;
};

class IPhysicsAccessor : public IComponentInterface
{
  public:
    virtual ~IPhysicsAccessor() = 0;
    virtual TEntityRef<ZSpatialEntity> GetSpatialOwner() const = 0;
    virtual bool IsPhysicsObject() const = 0;
    virtual IPhysicsObject* GetPhysicsObject() const = 0;
    virtual bool IsRegisteredForPhysicsObjectListening(
        const ZDelegate<void __cdecl(IPhysicsAccessor const*, IPhysicsObject const*, IPhysicsObject const*)>& param1
    ) const = 0;
    virtual void RegisterForPhysicsObjectListening(
        const ZDelegate<void __cdecl(IPhysicsAccessor const*, IPhysicsObject const*, IPhysicsObject const*)>& param1
    ) = 0;
    virtual void UnregisterForPhysicsObjectListening(
        const ZDelegate<void __cdecl(IPhysicsAccessor const*, IPhysicsObject const*, IPhysicsObject const*)>& param1
    ) = 0;
};

class IStaticPhysics : public IPhysicsAccessor
{
  public:
    virtual ~IStaticPhysics() = 0;
};

class NxUserContactModify
{
  public:
    struct NxContactCallbackData;

    virtual bool onContactConstraint(
        uint32_t& param1, const NxShape* nxShape, const NxShape* nxShape3, const uint32_t param4, const uint32_t param5,
        NxContactCallbackData& nxContactCallbackData
    ) = 0;
    virtual ~NxUserContactModify() = default;
};

class NxUserContactReport
{
  public:
    virtual void onContactNotify(NxContactPair& nxContactPair, uint32_t param2) = 0;
    virtual ~NxUserContactReport() = default;
};

struct SBoneCollision
{
    uint8_t m_nBoneId;
    float m_nT;
};

class ZRayQueryInput
{
  public:
    ZRayQueryInput(const float4& p_From, const float4& p_To, ERayDetailLevel p_RayDetailLevel)
    {
        m_vFrom = p_From;
        m_vTo = p_To;
        m_eRayDetailLevel = p_RayDetailLevel;
        m_nRayFilter = 0;
        m_eType = ECollidablesType::ECST_STATIC_AND_DYNAMIC;
        m_bIgnoreTransparentMaterials = false;
    }

    float4 m_vFrom;
    float4 m_vTo;
    uint32_t m_nRayFilter;
    ECollidablesType m_eType;
    ERayDetailLevel m_eRayDetailLevel;
    ZDelegate<bool __cdecl(ZEntityRef, TEntityRef<ZSpatialEntity>)> m_FilterCallback;
    bool m_bIgnoreTransparentMaterials;
};

class ZRayQueryOutputBase
{
  public:
    float4 m_vPosition;
    float4 m_vNormal;
    float m_nT;
    TResourcePtr<ZRenderMaterialInstance> m_pBlockingMaterial;
    uint8_t m_nIntersectedBoneMeshId;
    bool m_bHasHit;
};

class ZPhysicsObjectRef
{
  private:
    ZPhysicsObjectProxy* m_pProxy;
};

class ZRayQueryOutput : public ZRayQueryOutputBase
{
  public:
    ZEntityRef m_BlockingEntity;
    ZPhysicsObjectRef m_pBlockingPhysicsObject;
    TEntityRef<ZSpatialEntity> m_pBlockingSpatialEntity;
    TResourcePtr<ZRenderPrimitiveResource> m_pBlockingPrimitive;
    float4 m_vDecalPosition;
    float4 m_vDecalNormal;
    TArray<SBoneCollision> m_aAllIntersectedBones;
};

class ZCollisionManager : public ICollisionManager, public NxUserContactReport, public NxUserContactModify
{
  public:
    virtual void ZCollisionManager_Unk5() = 0;
    virtual void ZCollisionManager_Unk6() = 0;
    virtual void ZCollisionManager_Unk7() = 0;
    virtual void ZCollisionManager_Unk8() = 0;
    virtual void ZCollisionManager_Unk9() = 0;
    virtual void ZCollisionManager_Unk10() = 0;
    virtual void ZCollisionManager_Unk11() = 0;
    virtual void ZCollisionManager_Unk12() = 0;
    virtual void ZCollisionManager_Unk13() = 0;
    virtual void ZCollisionManager_Unk14() = 0;
    virtual void ZCollisionManager_Unk15() = 0;
    virtual bool RayCastClosestHit(const ZRayQueryInput& sInput, ZRayQueryOutput* pOutput) const = 0;
};

class ZPhysicsBaseEntity : public ZEntityImpl, public ZPresenceState
{
  public:
    virtual ~ZPhysicsBaseEntity() = 0;
    virtual void OnPhysicsWorldStateChanged() = 0;
};

class ZStaticPhysicsAspect : public ZPhysicsBaseEntity, public ICollisionShapeListener, public IStaticPhysics
{
  private:
    bool m_bRemovePhysics;
    IPhysicsObject* m_pPhysicsObject;
    ZEvent<IPhysicsAccessor const*, IPhysicsObject const*, IPhysicsObject const*> m_physicsObjectListeners;
};
