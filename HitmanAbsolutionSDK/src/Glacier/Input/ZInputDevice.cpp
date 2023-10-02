#include "Glacier/Input/ZInputDevice.h"
#include "Function.h"
#include "Global.h"

bool ZInputDevice::HandlerInstalled()
{
	int count = 0;

	for (auto i = m_pHandler; !*i; ++i)
	{
		if (++count >= 2)
		{
			return false;
		}
	}

	return true;
}

void ZInputDevice::SendToHandlers(int iKey, int iMask)
{
	if (m_nExclusiveIndex == -1)
	{
		auto handler = m_pHandler;

		for (int i = 2; i >= 0; --i)
		{
			if (*handler)
			{
				if (!(*handler)(iKey, iMask, handler[2]))
				{
					*handler = 0;
				}
			}

			++handler;
		}
	}
	else
	{
		m_pHandler[m_nExclusiveIndex](iKey, iMask, m_pHandlerData[m_nExclusiveIndex]);
	}
}

void ZInputDevice::buttonSample(int nr, bool state, const ZGameTime& time)
{
	if (nr >= 0 && nr < m_digcount)
	{
		if (m_pbDigitalTrap[nr])
		{
			m_pbDigitalTrap[nr] = state;
			state = 0;
		}

		if (m_pbDigital[nr] != state)
		{
			m_dighist[nr][1] = m_dighist[nr][0];
			m_dighist[nr][0] = time;
			m_pbDigital[nr] = state;
		}
	}
}

void ZInputDevice::AnalogSample(int nr, float state, float raw, const ZGameTime& time)
{
	m_anamotion[nr] = state - m_analog[nr];
	m_analog[nr] = state;
	m_anaraw[nr] = raw;
}

void ZInputDevice::AnalogMotionSample(int nr, float state, const ZGameTime& time)
{
	m_anamotion[nr] = state;
	m_analog[nr] += state;
}

const ZGameTime ZInputDevice::GetUpdateTime() const
{
    return m_updatetime;
}

const bool ZInputDevice::GetActive() const
{
    return m_active;
}

void ZInputDevice::SetUpdateTime(const ZGameTime& time)
{
    m_updatetime = time;
}
