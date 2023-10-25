#include <Global.h>
#include <Function.h>

bool SetPropertyValue(ZEntityType** pEntity, unsigned int nPropertyID, const ZVariantRef& value, bool bInvokeChangeHandlers)
{
	return Function::CallAndReturn<bool, ZEntityType**, unsigned int, const ZVariantRef&, bool>(BaseAddress + 0x82A20, pEntity, nPropertyID, value, bInvokeChangeHandlers);
}
