#pragma once

template <typename TElement, typename ZFixedArrayData>
class TFixedArrayBase : public ZFixedArrayData
{
public:
	unsigned int Size()
	{
		return this->m_pEnd - this->m_pStart;
	}

	unsigned int Size() const
	{
		return this->m_pEnd - this->m_pStart;
	}

	TElement* GetPointer()
	{
		return this->m_pStart;
	}

	TElement& operator[](unsigned int nIndex)
	{
		return this->m_pStart[nIndex];
	}

	const TElement& operator[](unsigned int nIndex) const
	{
		return this->m_pStart[nIndex];
	}
};
