#pragma once

#include "../TypeInfo/STypeID.h"

#include <Common.h>

class HitmanAbsolutionSDK_API ZObjectRef
{
public:
	ZObjectRef();
	ZObjectRef(void* pObject, STypeID* const typeID);
	STypeID* GetTypeID() const;
	void* GetData() const;
	const IType* GetType() const;

protected:
	STypeID* m_TypeID;
	void* m_pData;
};
