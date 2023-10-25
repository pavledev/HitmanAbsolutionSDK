#pragma once

#include "../Templates/TList.h"
#include "../Templates/TPair.h"
#include "../Templates/TArray.h"
#include "../ZDelegate.h"

template <typename T>
class ZEvent1
{
private:
	struct SInvocationData
	{
		SInvocationData* pNullOrDelegateAddedIndicator;
		unsigned int nRemoved;
		TList<TPair<int, ZDelegate<void __cdecl(T)>>> added;
	};

	TArray<TPair<int, ZDelegate<void __cdecl(T)>>> m_delegates;
	SInvocationData* m_pInvocation;
};
