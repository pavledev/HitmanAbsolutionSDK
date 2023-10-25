#pragma once

#include "../IComponentInterface.h"
#include "../Templates/TEntityRef.h"
#include "ZCheckPointManagerEntity.h"

class HitmanAbsolutionSDK_API ZCheckPointManager : public IComponentInterface
{
public:
	TEntityRef<ZCheckPointManagerEntity> GetCheckPointManagerEntity() const;

private:
	PAD(0x20);
	TEntityRef<ZCheckPointManagerEntity> m_pCheckPointManagerEntity;
};
