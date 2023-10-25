#pragma once

#include "TFixedArrayBase.h"
#include "ZArrayRefData.h"

template <typename T>
class TArrayRef : public TFixedArrayBase<T, ZArrayRefData<T>>
{
public:
	TArrayRef(T* pStart, unsigned int nSize)
	{
		this->m_pStart = pStart;
		this->m_pEnd = &pStart[nSize];
	}

	TArrayRef(T* pStart, T* pEnd)
	{
		this->m_pStart = pStart;
		this->m_pEnd = pEnd;
	}

	TArrayRef(const TArrayRef& rhs)
	{
		this->m_pStart = rhs->m_pStart;
		this->m_pEnd = rhs->m_pEnd;
	}
};
