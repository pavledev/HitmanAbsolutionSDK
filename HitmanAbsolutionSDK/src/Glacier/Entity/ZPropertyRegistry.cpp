#include <Glacier/Entity/ZPropertyRegistry.h>
#include <Glacier/Templates/THashMap.h>

#include <Global.h>

const ZString& ZPropertyRegistry::GetPropertyName(const unsigned int propertyID)
{
	if (!s_PropertyNameMap)
	{
		s_PropertyNameMap = *reinterpret_cast<THashMap<unsigned int, SPropertyName, TDefaultHashMapPolicy<unsigned int, SPropertyName>>**>(BaseAddress + 0xE253A8);
	}

	auto iterator = s_PropertyNameMap->Find(propertyID);

	if (iterator != s_PropertyNameMap->End())
	{
		return iterator->Value().m_sPropertyName;
	}

	static ZString propertyName;

	return propertyName;
}
