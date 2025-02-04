#include "Glacier/ZGameTimeManager.h"

const ZGameTime& ZGameTimeManager::GetRealTime() const
{
	return m_tRealTime;
}

void ZGameTimeManager::SetPaused(bool bPaused)
{
	m_bPaused = bPaused;
}
