#include <Glacier/Entity/ZAspectEntityBlueprintFactory.h>

IEntityBlueprintFactory* ZAspectEntityBlueprintFactory::GetAspectFactory(const unsigned int index) const
{
	return m_aspectFactories[index];
}
