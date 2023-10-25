#pragma once

#include "../Templates/TList.h"
#include "../Templates/TPair.h"
#include "../Templates/TArray.h"
#include "../ZDelegate.h"

class ZEvent0
{
private:
	struct SInvocationData
	{
		SInvocationData* pNullOrDelegateAddedIndicator;
		unsigned int nRemoved;
		TList<TPair<int, ZDelegate<void __cdecl(void)>>> added;
	};

	TArray<TPair<int, ZDelegate<void __cdecl(void)>>> m_delegates;
	SInvocationData* m_pInvocation;
};
