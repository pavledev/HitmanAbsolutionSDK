#include <Glacier/STokenID.h>

STokenID STokenID::InvalidToken{0, false};

bool STokenID::operator==(const STokenID& a) const
{
    if (m_bValid == a.m_bValid)
    {
        return m_iValue == a.m_iValue;
    }

    return !m_bValid && !a.m_bValid;
}

bool STokenID::operator!=(const STokenID& a) const
{
    return !(*this == a);
}
