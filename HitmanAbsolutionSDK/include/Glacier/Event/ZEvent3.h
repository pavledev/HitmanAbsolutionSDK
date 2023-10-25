#pragma once

#include "../Templates/TList.h"
#include "../Templates/TPair.h"
#include "../Templates/TArray.h"
#include "../ZDelegate.h"

template <typename T1, typename T2, typename T3>
class ZEvent3
{
private:
	struct SInvocationData
	{
		SInvocationData* pNullOrDelegateAddedIndicator;
		unsigned int nRemoved;
		TList<TPair<int, ZDelegate<void __cdecl(T1, T2, T3)>>> added;
	};

	TArray<TPair<int, ZDelegate<void __cdecl(T1, T2, T3)>>> m_delegates;
	SInvocationData* m_pInvocation;
};
