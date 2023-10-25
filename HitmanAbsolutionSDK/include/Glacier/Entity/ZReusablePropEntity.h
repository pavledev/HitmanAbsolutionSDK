#pragma once

#include "../Entity/ZEntityImpl.h"
#include "IReusableProp.h"

class ZComponentCreateInfo;

class ZReusablePropEntity : public ZEntityImpl, public IReusableProp
{
private:
	bool m_bUsedOnce;
};
