#pragma once

#include "STypeID.h"

class ZObjectRef
{
protected:
	STypeID* m_TypeID;
	void* m_pData;
};
