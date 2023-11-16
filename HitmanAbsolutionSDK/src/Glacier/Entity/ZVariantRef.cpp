#include <Glacier/Entity/ZVariantRef.h>
#include <Glacier/Entity/ZVariant.h>

ZVariantRef::ZVariantRef()
{
}

ZVariantRef::ZVariantRef(const ZVariant& rhs) : ZObjectRef(rhs.GetData(), rhs.GetTypeID())
{
}

ZVariantRef::ZVariantRef(void* pObject, STypeID* const typeID) : ZObjectRef(pObject, typeID)
{
}
