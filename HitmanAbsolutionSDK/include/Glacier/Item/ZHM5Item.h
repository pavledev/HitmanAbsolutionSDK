#pragma once

#include "../Entity/ZReusablePropEntity.h"
#include "IHM5Item.h"
#include "../AI/ZSharedKnowledgeRef.h"
#include "../Templates/TResourcePtr.h"
#include "../STokenID.h"

class IAnimPlayerEntity;
class ZHM5Action;

class ZHM5Item : public ZReusablePropEntity, public IHM5Item
{
public:
	virtual const ZResourcePtr& GetFactoryResource() const = 0;
	virtual void ResourceChanged() = 0;
	virtual void OnThrowImpact(const SCollisionInfo& collInfo) = 0;
	virtual void CheckFireOwnershipPins() = 0;
	virtual unsigned short GetCollisionLayer() const = 0;

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
