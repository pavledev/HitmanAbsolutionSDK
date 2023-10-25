#include <Glacier/STokenID.h>

STokenID STokenID::InvalidToken;

STokenID::STokenID()
{
    m_iValue = 0;
    m_bValid = false;
}

STokenID::STokenID(unsigned int iValue)
{
    m_iValue = iValue;
    m_bValid = true;
}

bool STokenID::operator==(const STokenID& a) const
{
    if (m_bValid && a.m_bValid)
    {
        return m_iValue == a.m_iValue;
    }

    return !m_bValid && !a.m_bValid;
}

bool STokenID::operator!=(const STokenID& a) const
{
    return !(*this == a);
}
