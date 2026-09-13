#pragma once

#include "TArray.h"
#include "TPair.h"
#include "ZBitArray.h"
#include "ZResource.h"
#include "ZString.h"
#include "Reflection.h"
#include "ZObject.h"
#include "CompileReflection.h"

#include <Common.h>
#include <Functions.h>
#include <Hash.h>

class ZVariantRef;
class IEntitySceneContext;
class ZGenericMemberFunctionTarget;
template<typename TKey, typename TValue> class THashSet;
template<typename T> class TDefaultHashSetPolicy;
struct SEntityStats;
struct SEntityProxyData;

enum EPropertyInfoFlags
{
    E_RUNTIME_EDITABLE = 1,
    E_CONST_AFTER_START = 2,
    E_STREAMABLE = 4,
    E_MEDIA_STREAMABLE = 8,
    E_HAS_GETTER_SETTER = 16
};

struct SPropertyInfo
{
    STypeID* m_Type;
    uint32_t m_nExtraData;
    uint32_t m_Flags;
    void (*m_PropertySetCallBack)(void* param1, void* param2, uint32_t param3, bool param4);
    void (*m_PropetyGetter)(void* param1, void* param2, uint32_t param3);
};

struct SPropertyData
{
    uint32_t m_nPropertyID;
    int32_t m_nPropertyOffset;
    const SPropertyInfo* m_pInfo;
};

struct SInterfaceData
{
    STypeID* m_Type;
    int32_t m_nInterfaceOffset;
};

struct SExposedEntityData
{
    ZString m_sExposedEntityName;
    int32_t m_nEntityOffset;
};

struct SSubsetData
{
    ZString m_sSubsetName;
    uint32_t m_nSubsetFlags;
    STypeID* m_subsetType;
    TArray<int32_t> m_aEntityOffsets;
    TArray<TPair<int32_t, SSubsetData*>> m_aEntitySubsets;
};

class ZPinFunctor
{
  public:
    void (*pfInvoke)(void (*param1)(), ZGenericMemberFunctionTarget* genericMemberFunctionTarget, const ZVariantRef& variantRef, uint32_t param4);
    void (*func)();
};

struct SPinInfo
{
    ZPinFunctor m_functor;
    uint32_t m_nExtraData;
};

struct SPinData
{
    uint32_t m_nPinID;
    int32_t m_nOffsetToThisPtr;
    const SPinInfo* m_pInfo;
};

class ZEntityRef;

class ZEntityType
{
  public:
    enum EntityTypeOwner
    {
        EOWNED_BY_FACTORY = 0,
        EOWNED_BY_ENTITY = 1
    };

    SInterfaceData* GetInterfaceData(const STypeID* p_Type)
    {
        if (!m_pInterfaceData)
        {
            return nullptr;
        }

        for (size_t i = 0; i < m_pInterfaceData->Size(); ++i)
        {
            if ((*m_pInterfaceData)[i].m_Type == p_Type)
            {
                return &(*m_pInterfaceData)[i];
            }
        }

        return nullptr;
    }

    SPropertyData* GetPropertyData(const uint32_t p_PropertyID)
    {
        if (!m_pPropertyData)
        {
            return nullptr;
        }

        for (size_t i = 0; i < m_pPropertyData->Size(); ++i)
        {
            if ((*m_pPropertyData)[i].m_nPropertyID == p_PropertyID)
            {
                return &(*m_pPropertyData)[i];
            }
        }

        return nullptr;
    }

    int32_t m_nBorrowedPointersMask;
    TArray<SPropertyData>* m_pPropertyData;
    TArray<SInterfaceData>* m_pInterfaceData;
    TArray<SExposedEntityData>* m_pExposedEntityData;
    TArray<SSubsetData>* m_pSubsets;
    TArray<SPinData>* m_pInputPins;
    TArray<SPinData>* m_pOutputPins;
    ZEvent<ZEntityRef const&, ZEventNull, ZEventNull, ZEventNull, ZEventNull>* m_pDeletionListeners;
};

class IEntity : public IComponentInterface
{
  public:
    virtual ~IEntity() = default;
    virtual ZEntityRef GetID() const = 0;
    virtual void Activate(const ZString& string) = 0;
    virtual void Deactivate(const ZString& string) = 0;
    virtual void StreamIn() = 0;
    virtual void StreamOut() = 0;
    virtual bool IsStreamedIn() = 0;
    virtual void GetStreamableIncludes(THashSet<ZEntityRef, TDefaultHashSetPolicy<ZEntityRef>>& hashSet) const = 0;
    virtual void OnEnterEditMode() = 0;
    virtual void OnExitEditMode() = 0;
};

class ZEntityImpl : public IEntity
{
  public:
    virtual void Init() = 0;
    virtual void Start() = 0;

    ZEntityType* m_pEntityType;
};

class ZEntityRef
{
  public:
    ZEntityRef() = default;

    ZEntityRef(ZEntityType** p_EntityTypePtrPtr) : m_pEntityTypePtrPtr(p_EntityTypePtrPtr) {}

    bool operator==(const ZEntityRef& p_Other) const
    {
        return m_pEntityTypePtrPtr == p_Other.m_pEntityTypePtrPtr;
    }

    bool operator!=(const ZEntityRef& p_Other) const
    {
        return m_pEntityTypePtrPtr != p_Other.m_pEntityTypePtrPtr;
    }

    ZEntityImpl* GetEntityImpl() const
    {
        if (m_pEntityTypePtrPtr)
        {
            return reinterpret_cast<ZEntityImpl*>(m_pEntityTypePtrPtr - 1);
        }

        return nullptr;
    }

    void* QueryInterfacePtr(const STypeID* p_InterfaceID) const
    {
        if (!m_pEntityTypePtrPtr)
        {
            return nullptr;
        }

        SInterfaceData* interfaceData = (*m_pEntityTypePtrPtr)->GetInterfaceData(p_InterfaceID);

        if (!interfaceData)
        {
            return nullptr;
        }

        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_pEntityTypePtrPtr) + interfaceData->m_nInterfaceOffset);
    }

    template<typename T> T* QueryInterfacePtr() const
    {
        if (!*Globals::TypeRegistry)
        {
            return nullptr;
        }

        const STypeID* typeID = (*Globals::TypeRegistry)->GetType(GlacierTypeName<T>);

        if (typeID)
        {
            return static_cast<T*>(QueryInterfacePtr(typeID));
        }

        return nullptr;
    }

    template<typename T> const bool HasInterface() const
    {
        if (!*Globals::TypeRegistry)
        {
            return false;
        }

        const STypeID* typeID = (*Globals::TypeRegistry)->GetType(GlacierTypeName<T>);

        if (typeID)
        {
            void* interfacePtr = QueryInterfacePtr(typeID);

            if (interfacePtr)
            {
                return true;
            }
        }

        return false;
    }

    const bool HasInterface(const std::string& p_TypeName) const
    {
        if (!*Globals::TypeRegistry)
        {
            return false;
        }

        const STypeID* typeID = (*Globals::TypeRegistry)->GetType(p_TypeName.c_str());

        if (typeID)
        {
            void* interfacePtr = QueryInterfacePtr(typeID);

            if (interfacePtr)
            {
                return true;
            }
        }

        return false;
    }

    ZVariant GetProperty(const ZString& p_PropertyName)
    {
        const uint32_t propertyID = hash::Crc32(p_PropertyName.ToCString(), p_PropertyName.Length());

        return GetProperty(propertyID);
    }

    ZVariant GetProperty(const uint32_t p_PropertyID)
    {
        ZVariant variant{};
        ZEntityType* entityType = (*m_pEntityTypePtrPtr);
        SPropertyData* propertyData = entityType->GetPropertyData(p_PropertyID);

        if (propertyData)
        {
            variant.Allocate(propertyData->m_pInfo->m_Type);

            const uint32_t propertyAddress = reinterpret_cast<uintptr_t>(m_pEntityTypePtrPtr) + propertyData->m_nPropertyOffset;

            if ((propertyData->m_pInfo->m_Flags & E_HAS_GETTER_SETTER) != 0)
            {
                propertyData->m_pInfo->m_PropetyGetter(
                    reinterpret_cast<void*>(propertyAddress), variant.m_pData, propertyData->m_pInfo->m_nExtraData
                );
            }
            else
            {
                propertyData->m_pInfo->m_Type->pTypeInfo->PlacementConstruct(variant.m_pData, reinterpret_cast<void*>(propertyAddress));
            }
        }

        return variant;
    }

    bool SetProperty(const uint32_t p_PropertyID, const ZVariantRef& p_Value, bool p_InvokeChangeHandlers = true)
    {
        return Functions::SetPropertyValue->Call(m_pEntityTypePtrPtr, p_PropertyID, p_Value, p_InvokeChangeHandlers);
    }

    bool SetProperty(const ZString& p_PropertyName, const ZVariantRef& p_Value, bool p_InvokeChangeHandlers = true)
    {
        return SetProperty(hash::Crc32(p_PropertyName.ToCString(), p_PropertyName.Length()), p_Value, p_InvokeChangeHandlers);
    }

    template<typename T> bool SetProperty(const uint32_t p_PropertyID, const T& p_Value, bool p_InvokeChangeHandlers = true)
    {
        ZVariant variant;
        variant.Set(p_Value);

        return Functions::SetPropertyValue->Call(m_pEntityTypePtrPtr, p_PropertyID, variant, p_InvokeChangeHandlers);
    }

    template<typename T> bool SetProperty(const ZString& p_PropertyName, const T& p_Value, bool p_InvokeChangeHandlers = true)
    {
        return SetProperty(hash::Crc32(p_PropertyName.ToCString(), p_PropertyName.Length()), p_Value, p_InvokeChangeHandlers);
    }

    template<typename T> bool SetProperty(uint32_t p_PropertyID, const ZVariant& p_Value, bool p_InvokeChangeHandlers = true)
    {
        return Functions::SetPropertyValue->Call(m_pEntityTypePtrPtr, p_PropertyID, p_Value, p_InvokeChangeHandlers);
    }

    template<typename T> bool SetProperty(const ZString& p_PropertyName, const ZVariant& p_Value, bool p_InvokeChangeHandlers = true)
    {
        return SetProperty(hash::Crc32(p_PropertyName.ToCString(), p_PropertyName.Length()), p_Value, p_InvokeChangeHandlers);
    }

    template<typename T> bool SetProperty(uint32_t p_PropertyID, const ZVariantRef& p_Value, bool p_InvokeChangeHandlers = true)
    {
        return Functions::SetPropertyValue->Call(m_pEntityTypePtrPtr, p_PropertyID, p_Value, p_InvokeChangeHandlers);
    }

    template<typename T> bool SetProperty(const ZString& p_PropertyName, const ZVariantRef& p_Value, bool p_InvokeChangeHandlers = true)
    {
        return SetProperty(hash::Crc32(p_PropertyName.ToCString(), p_PropertyName.Length()), p_Value, p_InvokeChangeHandlers);
    }

    void SignalInputPin(const ZString& p_PinName, const ZVariantRef& p_Data = ZVariantRef()) const
    {
        SignalInputPin(hash::Crc32(p_PinName.ToCString(), p_PinName.Length()), p_Data);
    }

    void SignalInputPin(uint32_t p_PinID, const ZVariantRef& p_Data = ZVariantRef()) const
    {
        Functions::SignalInputPin->Call(m_pEntityTypePtrPtr, p_PinID, p_Data);
    }

    void SignalOutputPin(const ZString& p_PinName, const ZVariantRef& p_Data = ZVariantRef()) const
    {
        SignalOutputPin(hash::Crc32(p_PinName.ToCString(), p_PinName.Length()), p_Data);
    }

    void SignalOutputPin(uint32_t p_PinID, const ZVariantRef& p_Data = ZVariantRef()) const
    {
        Functions::SignalOutputPin->Call(m_pEntityTypePtrPtr, p_PinID, p_Data);
    }

    ZEntityType** m_pEntityTypePtrPtr = nullptr;
};

template<typename T> class TEntityRef
{
  public:
    TEntityRef() = default;

    TEntityRef(const ZEntityRef& p_Other)
    {
        m_entityRef = p_Other;
        m_pInterfaceRef = p_Other.QueryInterfacePtr<T>();
    }

    TEntityRef(T* p_InterfaceRef)
    {
        m_entityRef = p_InterfaceRef->GetID();
        m_pInterfaceRef = p_InterfaceRef;
    }

    operator bool() const
    {
        return m_entityRef.m_pEntityTypePtrPtr && m_pInterfaceRef != nullptr;
    }

    ZEntityRef m_entityRef;
    T* m_pInterfaceRef;
};

class IEntityBlueprintFactory : public IComponentInterface
{
  public:
    virtual ~IEntityBlueprintFactory() = default;
    virtual void GetMemoryRequirements(uint32_t* param1, uint32_t* param2, uint32_t* param3) const = 0;
    virtual ZEntityType* GetFactoryEntityType() const = 0;
    virtual ZEntityType** CreateEntity(uint8_t* param1, ZEntityType* entityType) = 0;
    virtual void DestroyEntity(ZEntityType** entityType) const = 0;
    virtual void DisableReloadOfConstAfterStartResources(ZEntityType** entityType) const = 0;
    virtual bool AreAllResourcesReady(ZEntityType** entityType) const = 0;
    virtual void Start(ZEntityType** entityType) const = 0;
    virtual void CollectInternalEntities(ZEntityType** entityType, TArray<ZEntityRef>& array) const = 0;
    virtual void CollectEntityStats(SEntityStats& entityStats) const = 0;
    virtual void ClearAllEntityReferences(ZEntityType** entityType) const = 0;
    virtual void ClearAllEntityReferencesOnChildren(ZEntityType** entityType) const = 0;
    virtual void OnOrphanedResource() = 0;
};

class IEntityFactory : public IComponentInterface
{
  public:
    virtual ~IEntityFactory() = default;
    virtual void GetMemoryRequirements(uint32_t* param1, uint32_t* param2, uint32_t* param3) const = 0;
    virtual ZEntityType* GetFactoryEntityType() const = 0;
    virtual ZEntityType** CreateEntity(uint8_t* param1, ZEntityType* entityType) = 0;
    virtual void ConfigureEntity(ZEntityType** entityType) = 0;
    virtual void InitializeEntity(ZEntityType** entityType) = 0;
    virtual void DestroyEntity(ZEntityType** entityType) const = 0;
    virtual void DisableReloadOfConstAfterStartResources(ZEntityType** entityType) const = 0;
    virtual bool AreAllResourcesReady(ZEntityType** entityType) const = 0;
    virtual void Start(ZEntityType** entityType) const = 0;
    virtual void CollectInternalEntities(ZEntityType** entityType, TArray<ZEntityRef>& array) const = 0;
    virtual IEntityBlueprintFactory* GetBlueprint() const = 0;
    virtual TResourcePtr<IEntityBlueprintFactory> GetBlueprintResource() const = 0;
};

struct SEntityTemplateReference
{
    int32_t entityIndex;
    ZString exposedEntity;
};

struct SEntityTemplatePropertyAlias
{
    ZString sAliasName;
    int32_t entityID;
    ZString sPropertyName;
};

struct SEntityTemplateEntitySubset
{
    STypeID* subsetType;
    TArray<int32_t> entities;
};

struct SEntityTemplatePinConnection
{
    int32_t fromID;
    int32_t toID;
    ZString fromPinName;
    ZString toPinName;
};

struct STemplateSubEntityBlueprint
{
    int32_t parentIndex;
    int32_t entityTypeResourceIndex;
    ZString entityName;
    TArray<SEntityTemplatePropertyAlias> propertyAliases;
    TArray<TPair<ZString, SEntityTemplateReference>> exposedEntities;
    TArray<TPair<ZString, int32_t>> exposedInterfaces;
    TArray<TPair<ZString, SEntityTemplateEntitySubset>> entitySubsets;
};

struct STemplateEntityBlueprint
{
    int32_t rootEntityIndex;
    TArray<STemplateSubEntityBlueprint> entityTemplates;
    TArray<SEntityTemplatePinConnection> pinConnections;
    TArray<SEntityTemplatePinConnection> inputPinForwardings;
    TArray<SEntityTemplatePinConnection> outputPinForwardings;
};

class ZEntityBlueprintFactoryBase : public IEntityBlueprintFactory
{
  public:
    virtual ~ZEntityBlueprintFactoryBase() = default;
    virtual void OnOrphanedResource() = 0;

    uint32_t m_nNeededMemory;
    uint32_t m_nNeededAlignment;
    uint32_t m_nOffsetToType;
};

class ZCompositeEntityBlueprintFactoryBase : public ZEntityBlueprintFactoryBase
{
  public:
    virtual ~ZCompositeEntityBlueprintFactoryBase() = default;

    int32_t GetSubEntitiesCount() const
    {
        return m_aSubEntityOffsets.Size();
    }

    ZEntityType** GetSubEntity(ZEntityType** p_RootEntity, uint32_t p_Index) const
    {
        return reinterpret_cast<ZEntityType**>(reinterpret_cast<uintptr_t>(p_RootEntity) + m_aSubEntityOffsets[p_Index]);
    }

    TArray<uint32_t> m_aSubEntityOffsets;
};

class ZTemplateEntityBlueprintFactory : public ZCompositeEntityBlueprintFactoryBase
{
  public:
    virtual ~ZTemplateEntityBlueprintFactory() = default;

    int32_t GetSubEntitiesCount() const
    {
        return m_blueprintResources.Size();
    }

    ZEntityType** GetSubEntity(ZEntityType** p_Entity, int32_t p_Index) const
    {
        return reinterpret_cast<ZEntityType**>(reinterpret_cast<uintptr_t>(p_Entity) + m_aSubEntityOffsets[p_Index]);
    }

    ZEntityType* GetSubEntityType(uint32_t p_SubEntity) const
    {
        return m_blueprintResources[p_SubEntity]->GetFactoryEntityType();
    }

    IEntityBlueprintFactory* GetBlueprintResource(uint32_t p_EntityIndex) const
    {
        return m_blueprintResources[p_EntityIndex];
    }

    TArray<IEntityBlueprintFactory*> m_blueprintResources;
    TArray<ZEntityType*> m_aEntityTypes;
    ZBitArray m_aModifiedEntityType;
    int32_t m_rootEntityIndex;
    TArray<SPinInfo> m_aPinInfos;
    ZRuntimeResourceID m_ridResource;
};

class ZEntityReferenceBuffer
{
  public:
    uint32_t m_nEntityReferencePropertyDataSize;
    uint8_t* m_pEntityReferencePropertyData;
};

class ZTemplateEntityFactory : public IEntityFactory
{
  public:
    virtual ~ZTemplateEntityFactory() = default;

    struct SDirectlySettableProperty
    {
        uint32_t propertyOffset;
        ZVariantRef value;
    };

    struct SDirectlySettablePropertyWithSetter
    {
        uint32_t propertyOffset;
        const SPropertyInfo* pInfo;
        ZVariantRef value;
    };

    struct SResourceIDProperty
    {
        int32_t iEntity;
        uint32_t nPropertyID;
        ZRuntimeResourceID id;
    };

    TArray<SResourceIDProperty> m_resourceIDPropertyValues;
    ZEntityReferenceBuffer m_entityReferencePropertyBuffer;
    ZEntityReferenceBuffer m_postInitEntityReferencePropertyBuffer;
    TArray<SDirectlySettableProperty> m_directlySettableProperties;
    TArray<SDirectlySettablePropertyWithSetter> m_directlySettablePropertiesWithSetter;
    uint8_t* m_pDirectlySettablePropertyData;
    int32_t m_rootEntityIndex;
    TArray<IEntityFactory*> m_pFactories;
    TArray<ZRuntimeResourceID> m_ridReferencedResources;
    TArray<ZResourcePtr> m_referencedResources;
    ZRuntimeResourceID m_ridResource;
    TResourcePtr<ZTemplateEntityBlueprintFactory> m_blueprintResource;
};

class ZAspectEntityBlueprintFactory : public ZCompositeEntityBlueprintFactoryBase
{
  public:
    virtual ~ZAspectEntityBlueprintFactory() = default;

    ZEntityType* m_pFactoryEntityType;
    TArray<IEntityBlueprintFactory*> m_aspectFactories;
    SEntityProxyData* m_pEntityProxyData;
    ZRuntimeResourceID m_ridResource;
};

class ZAspectEntityFactory : public IEntityFactory
{
  public:
    virtual ~ZAspectEntityFactory() = default;

    TArray<TResourcePtr<IEntityFactory>> m_factoryResources;
    TResourcePtr<ZAspectEntityBlueprintFactory> m_blueprintResource;
    ZRuntimeResourceID m_ridResource;
};

class ZEntityManager : public IComponentInterface
{
  public:
    const IEntityBlueprintFactory* GetBlueprintFromCreatedEntity(ZEntityType** p_Entity) const
    {
        ZEntityRef entityRef = ZEntityRef(p_Entity);
        void* interfacePtr = entityRef.QueryInterfacePtr<void>();

        if (!interfacePtr)
        {
            return nullptr;
        }

        void* vFTable = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(interfacePtr) - sizeof(uintptr_t));

        if (vFTable == Globals::ZTemplateEntityBlueprintFactoryVFTbl)
        {
            return reinterpret_cast<IEntityBlueprintFactory*>(reinterpret_cast<uintptr_t>(interfacePtr) - sizeof(uintptr_t));
        }

        return nullptr;
    }

    IEntityBlueprintFactory* GetBlueprintFromCreatedEntity(ZEntityType** p_Entity)
    {
        return const_cast<IEntityBlueprintFactory*>(std::as_const(*this).GetBlueprintFromCreatedEntity(p_Entity));
    }

    IEntitySceneContext* m_pSceneContext;
    uint32_t m_nTransformParentPropertyID;
    uint32_t m_nObjectToParentID;
};
