#pragma once

#include "IComponentInterface.h"
#include "ZEntity.h"
#include "ZRender.h"
#include "ZMath.h"
#include "ZSharedKnowledgeRef.h"
#include "ZToken.h"

class ZItemFactoryEntity;
enum eItemSize;
enum eItemHands;
enum eItemType;
class ZString;
class ZEntityRef;
struct float4;
class ZLinkedEntity;
struct SCollisionInfo;
template<typename T> class TResourcePtr;
class ZDynamicFlashMovieResource;
class IPhysicsAccessor;
struct STokenID;
class ZContentKitEntity;
class IAnimPlayerEntity;

class IReusableProp
{
  public:
    virtual ~IReusableProp() = 0;
    virtual void UseOnce() = 0;
    virtual void ResetUsedFlag() = 0;
    virtual bool IsUsed() const = 0;
};

class ZReusablePropEntity : public ZEntityImpl, public IReusableProp
{
  public:
    bool m_bUsedOnce;
};

class IHM5Item : public IComponentInterface
{
  public:
    virtual void OnFactoryContructed(TEntityRef<ZItemFactoryEntity> entityRef) = 0;
    virtual eItemSize GetItemSize() const = 0;
    virtual eItemHands GetItemHands() const = 0;
    virtual eItemHands GetItemHandsCoverAnimLayer() const = 0;
    virtual eItemType GetItemType() const = 0;
    virtual const ZString& GetItemName() const = 0;
    virtual ZString GetItemTypeName() const = 0;
    virtual bool IsInventoryItem() const = 0;
    virtual bool IsInspectItem() const = 0;
    virtual void OnAttachItem(const ZEntityRef& entityRef) = 0;
    virtual void OnReleaseItem(bool param1) = 0;
    virtual void OnThrowItem(const float4& param1, const float4& param2) = 0;
    virtual void OnHolsterItem(const ZEntityRef& entityRef) = 0;
    virtual void EnablePickup(bool param1) = 0;
    virtual void ShowItem(bool param1) = 0;
    virtual bool ItemHidden() const = 0;
    virtual TEntityRef<ZSpatialEntity> GetSpatialEntity() const = 0;
    virtual TEntityRef<ZGeomEntity> GetGeomEntity() const = 0;
    virtual ZLinkedEntity* GetLinkedEntity() const = 0;
    virtual ZPrimitiveContainerEntity* GetPrimitiveContainerEntity() const = 0;
    virtual const ZEntityRef& GetOwner() const = 0;
    virtual void SetOwner(ZEntityRef entityRef) = 0;
    virtual void RemoveItemFromPhysicsWorld() = 0;
    virtual ZString GetDebugName() const = 0;
    virtual void Reset() = 0;
    virtual TEntityRef<ZSpatialEntity> GetHandAttacher() const = 0;
    virtual SMatrix GetHandAttacherOffset() const = 0;
    virtual void SetAIPerceptable(bool param1) = 0;
    virtual void SetThrown() = 0;
    virtual bool IsThrown() const = 0;
    virtual void OnCollision(const SCollisionInfo& collisionInfo, bool param2) = 0;
    virtual SMatrix GetMirroredGround() const = 0;
    virtual bool IsPercievedAsWeapon() const = 0;
    virtual bool CanBeDualWielded() const = 0;
    virtual TResourcePtr<ZDynamicFlashMovieResource> GetHUDIcon() const = 0;
    virtual TResourcePtr<ZDynamicFlashMovieResource> GetHUDSilencerIcon() const = 0;
    virtual const SVector2& GetSilencerPos() const = 0;
    virtual TEntityRef<IPhysicsAccessor> GetPhysicsAccessor() const = 0;
    virtual void SetFactoryResource(const TResourcePtr<IHM5Item>& resourcePtr) = 0;
    virtual const STokenID& GetTokenID() const = 0;
    virtual TEntityRef<ZContentKitEntity> GetContentKitEntity() const = 0;
    virtual void ContentKitEntityRetrieved() = 0;
    virtual ZEntityRef GetItemEntity() const = 0;
};

class ZHM5Item : public ZReusablePropEntity, public IHM5Item
{
  public:
    virtual const ZResourcePtr& GetFactoryResource() const = 0;
    virtual void ResourceChanged() = 0;
    virtual void OnThrowImpact(const SCollisionInfo& collInfo) = 0;
    virtual void CheckFireOwnershipPins() = 0;
    virtual uint16_t GetCollisionLayer() const = 0;

  private:
    eItemSize m_ItemSize;
    eItemHands m_ItemHands;
    eItemHands m_ItemHandsCoverAnimLayer;
    eItemType m_ItemType;
    ZEntityRef m_rPickupAction;
    TEntityRef<ZSpatialEntity> m_rGeomentity;
    TEntityRef<IPhysicsAccessor> m_rPhysicsAccessor;
    TEntityRef<ZSpatialEntity> m_PosHandAttach;
    TEntityRef<IAnimPlayerEntity> m_AnimPlayer;
    TResourcePtr<ZDynamicFlashMovieResource> m_pHUDIcon;
    TResourcePtr<ZDynamicFlashMovieResource> m_pHUDSilencerIcon;
    SVector2 m_vSilencerPos;
    bool m_bFixed;
    bool m_bEnablePickupInContracts;
    ZResourcePtr m_pFactoryResourcePtr;
    TResourcePtr<ZEntityRef> m_ContentKitEntity;
    STokenID m_TokenId;
    TEntityRef<ZGeomEntity> m_pGeomEntity;
    ZLinkedEntity* m_pGeomLinkedEntityInterface;
    ZPrimitiveContainerEntity* m_pPrimitiveContainerInterface;
    ZHM5Action* m_pPickupActionInterface;
    ZEntityRef m_pOwner;
    SMatrix m_MirroredGround;
    bool m_bEnablePickup : 1;
    bool m_bVisible : 1;
    bool m_bObjectInPhysicsWorld : 1;
    bool m_bThrown : 1;
    bool m_bActivated : 1;
    bool m_bRegisteredTriggerEntity : 1;
    ZSharedKnowledgeRef m_rAIObject;
    ZString m_sItemName;
    TEntityRef<ZItemFactoryEntity> m_pFactoryEntity;
};
