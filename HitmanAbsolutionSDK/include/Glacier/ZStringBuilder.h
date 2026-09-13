#pragma once

#include "TArray.h"
#include "ZString.h"

class ZStringBuilder
{
  public:
    ZStringBuilder()
    {
        m_chars.Reserve(256);
    }

    ZStringBuilder(const char* p_Initial, int32_t p_Length)
    {
        Initialize(p_Initial, p_Length);
    }

    ZStringBuilder(const ZString& p_Initial)
    {
        Initialize(p_Initial.ToCString(), p_Initial.Length() + 1);
    }

    void Initialize(const char* p_Data, int32_t p_Length)
    {
        m_chars.Reserve(256);

        if (p_Length == -1)
        {
            p_Length = strlen(p_Data);
        }

        if (p_Length > 0)
        {
            const size_t length = static_cast<size_t>(p_Length);

            if (length > m_chars.Capacity())
            {
                m_chars.Reserve(length);
            }

            m_chars.m_pEnd = m_chars.m_pEnd + length;

            memcpy(m_chars.GetStart(), p_Data, length);
        }
    }

    ZString ToString() const
    {
        return m_chars.GetStart();
    }

    TArray<char> m_chars;
};
