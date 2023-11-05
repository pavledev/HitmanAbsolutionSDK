#include <Glacier/Entity/ZEntityType.h>

SInterfaceData* ZEntityType::GetInterfaceData(const STypeID* type)
{
	if (!m_pInterfaceData)
	{
		return nullptr;
	}

	for (size_t i = 0; i < m_pInterfaceData->Size(); ++i)
	{
		if ((*m_pInterfaceData)[i].m_Type == type)
		{
			return &(*m_pInterfaceData)[i];
		}
	}

	return nullptr;
}

SPropertyData* ZEntityType::GetPropertyData(const unsigned int propertyID)
{
	if (!m_pPropertyData)
	{
		return nullptr;
	}

	for (size_t i = 0; i < m_pPropertyData->Size(); ++i)
	{
		if ((*m_pPropertyData)[i].m_nPropertyID == propertyID)
		{
			return &(*m_pPropertyData)[i];
		}
	}

	return nullptr;
}

TArray<SPropertyData>* ZEntityType::GetProperties()
{
	return m_pPropertyData;
}

TArray<SPinData>* ZEntityType::GetInputPins()
{
	return m_pInputPins;
}

TArray<SPinData>* ZEntityType::GetOutputPins()
{
	return m_pOutputPins;
}
