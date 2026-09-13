#pragma once

template<typename TKey, typename TValue> class TPair
{
  public:
    TPair(const TKey& p_Key, const TValue& p_Value) : m_key(p_Key), m_value(p_Value) {}

    bool operator==(const TPair& p_Other) const
    {
        return m_key == p_Other.m_key;
    }

    bool operator<(const TPair& p_Other) const
    {
        return m_key < p_Other.m_key;
    }

    TKey m_key;
    TValue m_value;
};
