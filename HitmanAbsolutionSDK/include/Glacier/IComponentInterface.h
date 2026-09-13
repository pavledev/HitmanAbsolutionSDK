#pragma once

class ZVariantRef;
struct STypeID;

class IComponentInterface
{
  public:
    virtual ~IComponentInterface() = default;
    virtual ZVariantRef GetVariantRef() const = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual void* QueryInterface(STypeID* typeID) = 0;
};
