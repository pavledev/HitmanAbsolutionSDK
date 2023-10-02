#include <Glacier/UI/ZGameWideUIScaleformHandler.h>

#include "Function.h"
#include "Global.h"

void ZGameWideUIScaleformHandler::ShowUICursor(bool bShow)
{
	return Function::CallMethod<ZGameWideUIScaleformHandler*, bool>(BaseAddress + 0x443840, this, bShow);
}
