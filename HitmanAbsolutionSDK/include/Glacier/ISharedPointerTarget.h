#pragma once

#include <Common.h>

class ISharedPointerTarget
{
  public:
    virtual ~ISharedPointerTarget() = default;
    virtual void AddReference() const = 0;
    virtual void RemoveReference() const = 0;
    virtual uint32_t GetRefCount() const = 0;
};
