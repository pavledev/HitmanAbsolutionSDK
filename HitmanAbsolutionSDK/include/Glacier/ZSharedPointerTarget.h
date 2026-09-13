#pragma once

#include "ISharedPointerTarget.h"

class ZSharedPointerTarget : public ISharedPointerTarget
{
  public:
    long m_iRefCount;
};
