#pragma once

#include "IComponentInterface.h"

#include <Functions.h>

class ZGlobalBoneRegistry : public IComponentInterface
{
  public:
    int32_t GetBoneID(char const* p_BoneName) const
    {
        return Functions::ZGlobalBoneRegistry_GetBoneID->Call(this, p_BoneName);
    }
};
