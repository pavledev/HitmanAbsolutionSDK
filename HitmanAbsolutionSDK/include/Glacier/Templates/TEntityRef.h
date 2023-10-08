#pragma once

#include "../Entity/ZEntityRef.h"

template <typename T>
class TEntityRef
{
public:
	ZEntityRef GetEntityRef() const
	{
		return m_entityRef;
	}

	T* GetRawPointer() const
	{
		return m_pInterfaceRef;
	}

	T* operator->()
	{
		return GetRawPointer();
	}

private:
	ZEntityRef m_entityRef;
	T* m_pInterfaceRef;
};
