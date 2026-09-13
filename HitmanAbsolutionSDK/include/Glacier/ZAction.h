#pragma once

#include "ZEntity.h"
#include "ZResource.h"
#include "TSList.h"
#include "ZGameTime.h"
#include "ZMath.h"

class ZBoxVolumeEntity;
class IHM5Item;
class ZHM5BaseMovement;
class ZIllegalActionEntity;
class ZSpatialEntity;
class ZTextListData;
class ZAABBTreeNode;

enum EActionType
{
    AT_INVALID = 0,
    AT_OPENDOOR = 1,
    AT_CLOSEDOOR = 2,
    AT_PICKUP = 4,
    AT_STARTDIALOG = 8,
    AT_RESET = 16,
    AT_OUTFIT = 32,
    AT_CLOTHBUNDLE = 64,
    AT_DRAGBODY = 128,
    AT_HUMANSHIELD = 256,
    AT_RELEASEHUMANSHIELD = 512,
    AT_SWITCHBOX = 1024,
    AT_SWAPITEM = 2048,
    AT_DUMPBODY = 4096,
    AT_OPENLID = 8192,
    AT_CLOSELID = 16384,
    AT_CHECKPOINTDOOR = 32768,
    AT_SIMPLE = 65536,
    AT_ENTERCLOSET = 131072,
    AT_EXITCLOSET = 262144,
    AT_ENTERSAFEZONE = 524288,
    AT_EXITSAFEZONE = 1048576,
    AT_DISABLEFUSEBOX = 2097152,
    AT_CONTAINERFLUSHBODY = 4194304,
    AT_VENTILATORSHAFT = 8388608,
    AT_HEALTHSTATION = 16777216,
    AT_KEYCARDREADER = 33554432,
    AT_USE = 67108864,
    AT_CONTEXTACTION = 134217728,
    AT_KILLHUMANSHIELD = 268435456
};

class ZHM5Action : public ZEntityImpl
{
  public:
    EActionType m_eActionType;
    ZEntityRef m_Object;
    TEntityRef<ZBoxVolumeEntity> m_rActivateBoxVolume;
    ZString m_sActionName;
    bool m_bVisible;
    ZEntityRef m_Listener;
    ZString m_sDefaultItemName;
    TResourcePtr<ZTextListData> m_pTextListResource;
    ZString m_sExitTextID;
    ZString m_sCachedString;
    ZString m_sCachedExitString;
    TEntityRef<ZSpatialEntity> m_3dPromptPosition;
    TEntityRef<ZIllegalActionEntity> m_rIllegalActionEntity;
    TEntityRef<IHM5Item> m_rUserDataItem;
    ZHM5BaseMovement* m_pActionMovement;
    float4 m_vObjectCenter;
    float4 m_vObjectSize;
    bool m_bObjectDirty;
    bool m_bCallbackRegistered;
    ZEntityRef m_OwnListener;
    ZEntityRef m_rVentShaftInstance;
    float m_fButtonHeightOffset;
};

class ZHM5ActionManager : public IComponentInterface
{
  public:
    struct SActionTreeEntry
    {
        ZHM5Action* m_pNodeAction;
        void* m_pObjectNode;
    };

    TSList<SActionTreeEntry> m_Actions;
    ZAABBTreeNode* m_pAABBActionTree;
    TSList<ZHM5Action*> m_ObjectlessActions;
    ZGameTime m_LastUpdate;
};
