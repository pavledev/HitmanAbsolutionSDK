#include <Glacier/STokenID.h>

STokenID STokenID::InvalidToken{0, false};

bool STokenID::operator==(const STokenID& a) const
{
    if (!m_bValid || !a.m_bValid)
    {
        return false;
    }

    return m_iValue == a.m_iValue;
}

bool STokenID::operator!=(const STokenID& a) const
{
    return !(*this == a);
}
