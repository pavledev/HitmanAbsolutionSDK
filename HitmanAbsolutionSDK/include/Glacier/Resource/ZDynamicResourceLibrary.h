#pragma once

#include "ZRuntimeResourceID.h"
#include "ZResourceLibrarySet.h"
#include "../Templates/TResourcePtr.h"
#include "../Entity/ZEntityRef.h"
#include "ZHeaderLibrary.h"
#include "../Event/ZEvent.h"

class HitmanAbsolutionSDK_API ZDynamicResourceLibrary
{
public:
	ZDynamicResourceLibrary(const ZRuntimeResourceID& ridHeaderLibrary, bool bDelayedCreateEntities, unsigned int nNumInstances);
	~ZDynamicResourceLibrary();
	ZRuntimeResourceID GetLibraryResourceID();
	ZEntityRef GetEntity(int nIndex);
	void WaitForResources();
	bool IsReady();
	bool IsFailed();
	unsigned int GetEntityCount() const;
	void CreateEntities();
	void DeleteAllEntities();
	void OnHeaderLibraryReady(const ZRuntimeResourceID& rid);
	void OnResourceLibrariesReady();

private:
	ZRuntimeResourceID m_ridHeaderLibrary;
	ZResourceLibrarySet* m_pCurrentLibrarySet;
	TResourcePtr<ZHeaderLibrary> m_HeaderLibrary;
	ZEvent<ZRuntimeResourceID const&, ZEventNull, ZEventNull, ZEventNull, ZEventNull> m_LibraryReadyEvent;
	TArray<ZEntityRef> m_Entities;
	const unsigned int m_iEntitiesNUM;
	bool m_bReady : 1;
	bool m_bFailed : 1;
	bool m_bLoadingHeader : 1;
	bool m_bDelayedCreateEntities : 1;
};
