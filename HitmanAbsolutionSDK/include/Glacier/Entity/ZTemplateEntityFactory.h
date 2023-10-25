#pragma once

#include "IEntityFactory.h"
#include "ZEntityReferenceBuffer.h"
#include "ZTemplateEntityBlueprintFactory.h"
#include "ZVariantRef.h"

class HitmanAbsolutionSDK_API ZTemplateEntityFactory : public IEntityFactory
{
public:
	virtual ~ZTemplateEntityFactory() = default;

	ZRuntimeResourceID GetTemplateRuntimeResourceID() const;

private:
	struct SDirectlySettableProperty
	{
		unsigned int propertyOffset;
		ZVariantRef value;
	};

	struct SDirectlySettablePropertyWithSetter
	{
		unsigned int propertyOffset;
		const SPropertyInfo* pInfo;
		ZVariantRef value;
	};

	struct SResourceIDProperty
	{
		int iEntity;
		unsigned int nPropertyID;
		ZRuntimeResourceID id;
	};

	TArray<SResourceIDProperty> m_resourceIDPropertyValues;
	ZEntityReferenceBuffer m_entityReferencePropertyBuffer;
	ZEntityReferenceBuffer m_postInitEntityReferencePropertyBuffer;
	TArray<SDirectlySettableProperty> m_directlySettableProperties;
	TArray<SDirectlySettablePropertyWithSetter> m_directlySettablePropertiesWithSetter;
	unsigned char* m_pDirectlySettablePropertyData;
	int m_rootEntityIndex;
	TArray<IEntityFactory*> m_pFactories;
	TArray<ZRuntimeResourceID> m_ridReferencedResources;
	TArray<ZResourcePtr> m_referencedResources;
	ZRuntimeResourceID m_ridResource;
	TResourcePtr<ZTemplateEntityBlueprintFactory> m_blueprintResource;
};
