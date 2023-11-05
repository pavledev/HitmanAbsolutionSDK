#pragma once

template <typename T>
class TSharedPointer
{
public:
	T* operator->() const
	{
		return m_pObject;
	}

	const T* GetTarget() const
	{
		return m_pObject;
	}

private:
	T* m_pObject;
};
