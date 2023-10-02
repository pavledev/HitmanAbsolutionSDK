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
