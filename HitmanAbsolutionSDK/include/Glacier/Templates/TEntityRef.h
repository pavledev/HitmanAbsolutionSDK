#pragma once

#include "../Entity/ZEntityRef.h"

template <typename T>
class TEntityRef
{
public:
	T* GetRawPointer() const
	{
		return m_pInterfaceRef;
	}

private:
	ZEntityRef m_entityRef;
	T* m_pInterfaceRef;
};
