#pragma once

#include "ZEntity.h"

class ZCheckPointManagerEntity : public ZEntityImpl
{
  public:
    void ActivateJumpPoint(int32_t p_Index, bool p_ResetHitman)
    {
        Functions::ZCheckPointManagerEntity_ActivateJumpPoint->Call(this, p_Index, p_ResetHitman);
    }

    PAD(0x84);
    int32_t m_iCurrentJumpPoint;
};

class ZCheckPointManager : public IComponentInterface
{
  public:
    PAD(0x20);
    TEntityRef<ZCheckPointManagerEntity> m_pCheckPointManagerEntity;
};
