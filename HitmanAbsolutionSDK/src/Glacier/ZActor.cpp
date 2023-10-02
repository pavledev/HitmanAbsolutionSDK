#include <Glacier/ZActor.h>

#include <Function.h>
#include <Global.h>

void ZActor::KillActor(EActorDeathType eDeathType, bool bDeathVisible)
{
	Function::CallMethod<ZActor*, EActorDeathType, bool>(BaseAddress + 0x174790, this, eDeathType, bDeathVisible);
}
