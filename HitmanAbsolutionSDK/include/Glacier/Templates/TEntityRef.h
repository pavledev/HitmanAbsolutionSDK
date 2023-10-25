#pragma once

#include "../Entity/ZEntityRef.h"

template <typename T>
class TEntityRef
{
public:
	TEntityRef()
	{
		m_pInterfaceRef = nullptr;
	}

	TEntityRef(const ZEntityRef& rhs)
	{
		m_entityRef = rhs;
		m_pInterfaceRef = rhs.QueryInterfacePtr<T>();
	}

	TEntityRef(T* interfaceRef)
	{
		m_entityRef = interfaceRef->GetID();
		m_pInterfaceRef = interfaceRef;
	}

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
