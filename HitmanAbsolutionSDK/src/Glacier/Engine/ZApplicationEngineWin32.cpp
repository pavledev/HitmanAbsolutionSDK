#include <Glacier/Engine/ZApplicationEngineWin32.h>

#include "Global.h"

ZApplicationEngineWin32* ZApplicationEngineWin32::GetInstance()
{
	return *m_pInstance;
}

void ZApplicationEngineWin32::SetInstance(ZApplicationEngineWin32** instance)
{
	m_pInstance = instance;
}

ZEngineAppCommon& ZApplicationEngineWin32::GetEngineAppCommon()
{
	return m_common;
}

HWND ZApplicationEngineWin32::GetHWND()
{
	return m_hWnd;
}
