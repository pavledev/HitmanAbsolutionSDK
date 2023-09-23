#pragma once

#include "IEntity.h"
#include "ZEntityType.h"

class ZEntityImpl : public IEntity
{
private:
	ZEntityType* m_pEntityType;
};
