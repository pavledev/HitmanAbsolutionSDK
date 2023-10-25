#pragma once

#include "../Templates/TList.h"
#include "../Templates/TPair.h"
#include "../Templates/TArray.h"
#include "../ZDelegate.h"

template <typename T1, typename T2>
class ZEvent2
{
private:
	struct SInvocationData
	{
		SInvocationData* pNullOrDelegateAddedIndicator;
		unsigned int nRemoved;
		TList<TPair<int, ZDelegate<void __cdecl(T1, T2)>>> added;
	};

	TArray<TPair<int, ZDelegate<void __cdecl(T1, T2)>>> m_delegates;
	SInvocationData* m_pInvocation;
};
