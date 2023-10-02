#pragma once

#include "../TypeInfo/STypeID.h"

class ZObjectRef
{
public:
	ZObjectRef(void* pObject, STypeID* const typeID);
	STypeID* GetTypeID() const;
	void* GetData() const;
	const IType* GetType() const;

protected:
	STypeID* m_TypeID;
	void* m_pData;
};
