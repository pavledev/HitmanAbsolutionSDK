#pragma once

#include "../Entity/ZEntityImpl.h"
#include "EActionType.h"
#include "../Templates/TEntityRef.h"
#include "../Templates/TResourcePtr.h"
#include "../Entity/ZEntityRef.h"
#include "../ZString.h"
#include "../Math/float4.h"

class ZBoxVolumeEntity;
class IHM5Item;
class ZHM5BaseMovement;
class ZIllegalActionEntity;
class ZSpatialEntity;
class ZTextListData;

class HitmanAbsolutionSDK_API ZHM5Action : public ZEntityImpl
{
public:
	EActionType GetActionType() const;
	TEntityRef<IEntity> GetActionObject() const;

private:
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
