#pragma once

template <typename TKey, typename TValue>
class TPair
{
public:
	TPair(const TKey& key, const TValue& value) : m_key(key), m_value(value)
	{
	}

	const TKey& Key() const
	{
		return m_key;
	}

	const TValue& Value() const
	{
		return m_value;
	}

	bool operator==(const TPair& rhs) const
	{
		return m_key == rhs.m_key;
	}

	bool operator<(const TPair& rhs) const
	{
		return m_key < rhs.m_key;
	}

private:
	TKey m_key;
	TValue m_value;
};
