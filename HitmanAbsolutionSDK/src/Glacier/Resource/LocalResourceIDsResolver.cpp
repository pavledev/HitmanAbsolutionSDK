#include <Glacier/Resource/LocalResourceIDsResolver.h>

#include <Global.h>
#include <Function.h>

void LocalResourceIDsResolver::RecordMapping(const ZRuntimeResourceID& ridResource, const ZResourceID& idResource)
{
	Function::CallMethod<LocalResourceIDsResolver*, const ZRuntimeResourceID&, const ZResourceID&>(BaseAddress + 0x354600, this, ridResource, idResource);
}
