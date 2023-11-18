#include <Global.h>
#include <Function.h>

bool SetPropertyValue(ZEntityType** pEntity, unsigned int nPropertyID, const ZVariantRef& value, bool bInvokeChangeHandlers)
{
	return Function::CallAndReturn<bool, ZEntityType**, unsigned int, const ZVariantRef&, bool>(BaseAddress + 0x82A20, pEntity, nPropertyID, value, bInvokeChangeHandlers);
}

void SignalInputPin(ZEntityType** pEntity, unsigned int nPinID, const ZVariantRef& data)
{
	return Function::Call<ZEntityType**, unsigned int, const ZVariantRef&>(BaseAddress + 0x576C0, pEntity, nPinID, data);
}

void SignalOutputPin(ZEntityType** pEntity, unsigned int nPinID, const ZVariantRef& data)
{
	return Function::Call<ZEntityType**, unsigned int, const ZVariantRef&>(BaseAddress + 0x19BBA0, pEntity, nPinID, data);
}

bool GetApplicationOptionBool(const ZString& sName, bool bDefault)
{
	return Function::CallAndReturn<bool, const ZString&, bool>(BaseAddress + 0x212930, sName, bDefault);
}
