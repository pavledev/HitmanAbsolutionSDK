#include "Glacier/Input/ZDirectInputInf.h"

IDirectInputDevice8A* ZDirectInputInf::GetDirectInputDevice() const
{
	return m_pDev;
}

const bool ZDirectInputInf::GetAcquired() const
{
	return m_bAcquired;
}

void ZDirectInputInf::SetAcquired(const bool acquired)
{
	m_bAcquired = acquired;
}
