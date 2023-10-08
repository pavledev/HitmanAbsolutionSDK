#pragma once

#include "ZContentKitEntity.h"
#include "../Templates/TEntityRef.h"
#include "../Resource/ZRuntimeResourceID.h"

class ZHM5WeaponBasicConfigEntity;

class HitmanAbsolutionSDK_API ZFireArmKitEntity : public ZContentKitEntity
{
public:
	ZRuntimeResourceID GetLargeIconRID() const;

private:
	TEntityRef<ZHM5WeaponBasicConfigEntity> m_BasicConfig;
	ZRuntimeResourceID m_sHiResNotebookImage;
	bool m_bIsNewPickup;
};
