#pragma once

template <typename T>
class TSharedPointer
{
public:
	TSharedPointer() = default;

	~TSharedPointer()
	{
		if (m_pObject)
		{
			m_pObject->RemoveReference();
		}

		m_pObject = nullptr;
	}

	TSharedPointer(const TSharedPointer& rhs)
	{
		m_pObject = rhs.m_pObject;

		if (m_pObject)
		{
			m_pObject->AddReference();
		}
	}

	TSharedPointer& operator=(const TSharedPointer& rhs)
	{
		if (m_pObject != rhs.m_pObject)
		{
			if (m_pObject)
			{
				m_pObject->RemoveReference();
			}

			m_pObject = rhs.m_pObject;

			if (m_pObject)
			{
				m_pObject->AddReference();
			}
		}

		return *this;
	}

	T* operator->() const
	{
		return m_pObject;
	}

	T* operator->()
	{
		return m_pObject;
	}

	const T* GetTarget() const
	{
		return m_pObject;
	}

	T* GetTarget()
	{
		return m_pObject;
	}

	void SetTarget(T* object)
	{
		m_pObject = object;
	}

private:
	T* m_pObject;
};
