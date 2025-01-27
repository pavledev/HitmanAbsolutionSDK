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

void ZGameLoopManager::SetPlayMode(EPlayMode ePlayMode)
{
	Function::CallMethod<ZGameLoopManager*, EPlayMode>(BaseAddress + 0x49A8C0, this, ePlayMode);
}
