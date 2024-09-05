#include "Glacier/ZGameTime.h"

long long ZGameTime::GetTicks() const
{
	return m_nTicks;
}

double ZGameTime::ToSeconds() const
{
    return static_cast<double>(m_nTicks) / 1024.0 / 1024.0;
}
