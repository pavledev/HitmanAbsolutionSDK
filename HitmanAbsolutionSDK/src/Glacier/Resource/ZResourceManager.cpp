#include <Glacier/Resource/ZResourceManager.h>

#include <Global.h>
#include <Function.h>

ZResourcePtr ZResourceManager::GetResourcePtr(const ZRuntimeResourceID& ridResource, int nPriority)
{
	return Function::CallRVOMethodAndReturn<ZResourcePtr, ZResourceManager*, const ZRuntimeResourceID&, int>(BaseAddress + 0x225BE0, this, ridResource, nPriority);
}

ZResourcePtr ZResourceManager::LoadResource(const ZRuntimeResourceID& ridResource)
{
	return Function::CallRVOMethodAndReturn<ZResourcePtr, ZResourceManager*, const ZRuntimeResourceID&>(BaseAddress + 0x48B520, this, ridResource);
}

void ZResourceManager::Update(bool bSendStatusChangedNotifications)
{
	Function::CallMethod<ZResourceManager*, bool>(BaseAddress + 0x3DD4B0, this, bSendStatusChangedNotifications);
}

void ZResourceManager::ReleaseStub(ZResourceStub* stub)
{
	Function::CallMethod<ZResourceManager*, ZResourceStub*>(BaseAddress + 0x34EFE0, this, stub);
}
