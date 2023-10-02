#include <Glacier/ZGameLoopManager.h>

#include <Function.h>
#include <Global.h>

void ZGameLoopManager::RegisterForFrameUpdate(const ZDelegate<void __cdecl(SGameUpdateEvent const&)>& callback, int nPriority)
{
	Function::CallMethod<ZGameLoopManager*, const ZDelegate<void __cdecl(SGameUpdateEvent const&)>&, int>(BaseAddress + 0x8CC00, this, callback, nPriority);
}

void ZGameLoopManager::UnregisterForFrameUpdate(const ZDelegate<void __cdecl(SGameUpdateEvent const&)>& callback)
{
	Function::CallMethod<ZGameLoopManager*, const ZDelegate<void __cdecl(SGameUpdateEvent const&)>&>(BaseAddress + 0x59EFF0, this, callback);
}
