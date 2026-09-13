#pragma once

#include "ZEntity.h"

#include <Common.h>
#include <Hash.h>
#include <Utils/StringUtils.h>

struct STokenID
{
    STokenID() : m_iValue(0), m_bValid(false) {}

    STokenID(uint32_t p_Value) : m_iValue(p_Value), m_bValid(true) {}

    bool operator==(const STokenID& p_Other) const
    {
        if (m_bValid && p_Other.m_bValid)
        {
            return m_iValue == p_Other.m_iValue;
        }

        return !m_bValid && !p_Other.m_bValid;
    }

    bool operator!=(const STokenID& p_Other) const
    {
        return !(*this == p_Other);
    }

    bool operator<(const STokenID& p_Other) const
    {
        return m_bValid && p_Other.m_bValid && m_iValue < p_Other.m_iValue;
    }

    uint32_t m_iValue;
    bool m_bValid;

    static const STokenID InvalidToken;
};

inline const STokenID STokenID::InvalidToken{};

class ZTokenIDEntity : public ZEntityImpl
{
  public:
    const STokenID& GetTokenID()
    {
        if (m_TokenIDCached == STokenID::InvalidToken)
        {
            std::string name = util::ToUpperCase(m_sName.ToCString());

            m_TokenIDCached.m_iValue = hash::Crc32(name.c_str(), name.length());
            m_TokenIDCached.m_bValid = true;
        }

        return m_TokenIDCached;
    }

    ZString m_sName;
    STokenID m_TokenIDCached;
};
