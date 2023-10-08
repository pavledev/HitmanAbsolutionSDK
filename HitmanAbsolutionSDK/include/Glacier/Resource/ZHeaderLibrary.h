#pragma once

#include "../Templates/TArray.h"
#include "ZResourceStub.h"
#include "../Templates/THashMap.h"
#include "ZResourceLibraryInfo.h"

template <typename A, class B, class C, class D, class E> class ZEvent;
class ZEventNull;

class HitmanAbsolutionSDK_API ZHeaderLibrary
{
public:
	ZResourcePtr GetSourceResource() const;

private:
	const TArray<ZResourceLibraryInfo*> m_LibraryLoadOrder;
	ZResourceStub* m_pHeaderLibraryStub;
	THashMap<ZRuntimeResourceID, ZEvent<ZRuntimeResourceID const&, ZEventNull, ZEventNull, ZEventNull, ZEventNull>, TDefaultHashMapPolicy<ZRuntimeResourceID, ZEvent<ZRuntimeResourceID const&, ZEventNull, ZEventNull, ZEventNull, ZEventNull>>> m_ResourceUnloadedEvents;
};
