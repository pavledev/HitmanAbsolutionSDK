#include <Glacier/Input/ZHM5InputControl.h>
#include <Glacier/Input/ZInputAction.h>

#include "Global.h"

void ZHM5InputControl::EnableInputControl(bool bValue)
{
	m_bActive = bValue;
}

void ZHM5InputControl::EnableBindings()
{
	if (m_nBindingsDisabledCount-- == 1)
	{
		HM5InputControl->SetEnabled(true);
	}
}

void ZHM5InputControl::DisableBindings()
{
	++m_nBindingsDisabledCount;

	HM5InputControl->SetEnabled(false);
}
