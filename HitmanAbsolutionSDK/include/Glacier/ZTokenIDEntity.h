#pragma once

#include "Entity/ZEntityImpl.h"
#include "STokenID.h"

class ZTokenIDEntity : public ZEntityImpl
{
public:
	const STokenID& GetTokenID();

private:
	ZString m_sName;
	STokenID m_TokenIDCached;
};
