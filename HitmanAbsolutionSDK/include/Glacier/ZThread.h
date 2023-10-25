#pragma once

#include "ZDelegate.h"
#include "ZString.h"

class ZThread
{
public:
	static void Sleep(unsigned int nMsecs);

private:
	ZDelegate<unsigned int __cdecl(void*)> m_function;
	int m_handle;
	ZString m_sName;
	unsigned int m_nExitCode;
	int m_nStackSize;
	unsigned char m_nJoinState;
};
