#pragma once

#include "../ZMutex.h"
#include "../Templates/TArray.h"
#include "ZResourcePtr.h"
#include "../ZDelegate.h"

class ZResourceLibrarySet
{
public:
	void RemoveReadyCallback(const ZDelegate<void __cdecl(void)>& d);
	void Release();

private:
	ZMutex m_Mutex;
	ZMutex m_ExecutingCallbackMutex;
	TArray<ZResourcePtr> m_Libraries;
	TArray<ZDelegate<void __cdecl(void)>> m_Callbacks;
	ZResourcePtr m_pHeaderLibrary;
	bool m_bReleased : 1;
	bool m_bProcessingCallbacks : 1;
};
