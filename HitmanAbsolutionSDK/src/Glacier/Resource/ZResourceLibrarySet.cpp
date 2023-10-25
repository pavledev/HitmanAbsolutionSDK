#include <Glacier/Resource/ZResourceLibrarySet.h>

#include <Global.h>
#include <Function.h>

void ZResourceLibrarySet::RemoveReadyCallback(const ZDelegate<void __cdecl(void)>& d)
{
	Function::CallMethod<ZResourceLibrarySet*, const ZDelegate<void __cdecl(void)>&>(BaseAddress + 0x40D4E0, this, d);
}

void ZResourceLibrarySet::Release()
{
	Function::CallMethod<ZResourceLibrarySet*>(BaseAddress + 0x5E1CD0, this);
}
