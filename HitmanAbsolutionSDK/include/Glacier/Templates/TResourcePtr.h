#pragma once

#include "../Resource/ZResourcePtr.h"

template <typename T>
class TResourcePtr : public ZResourcePtr
{
public:
	TResourcePtr() = default;

	TResourcePtr(const ZResourcePtr& rhs) : ZResourcePtr(rhs)
	{
	}

	T* GetRawPointer() const
	{
		return static_cast<T*>(ZResourcePtr::GetRawPointer());
	}

	T* operator->()
	{
		return GetRawPointer();
	}
};
