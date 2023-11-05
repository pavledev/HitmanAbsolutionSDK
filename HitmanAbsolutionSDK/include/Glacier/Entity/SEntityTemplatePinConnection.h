#pragma once

#include "../ZString.h"

struct SEntityTemplatePinConnection
{
	int fromID;
	int toID;
	ZString fromPinName;
	ZString toPinName;
};
