#pragma once

#include "ZMutex.h"
#include "THashMap.h"
#include "ZMemory.h"
#include "ZString.h"
#include "CompileReflection.h"

#include <Globals.h>

enum ETypeInfoFlags
{
    ETYPEINFOFLAG_TENTITYREF = 1,
    ETYPEINFOFLAG_TRESOURCEPTR = 2,
    ETYPEINFOFLAG_CLASSTYPE = 4,
    ETYPEINFOFLAG_ENUMTYPE = 8,
    ETYPEINFOFLAG_CONTAINERTYPE = 16,
    ETYPEINFOFLAG_ARRAYTYPE = 32,
    ETYPEINFOFLAG_FIXEDARRAYTYPE = 64
};

struct STypeFunctions
{
    void (*placementConstruct)(void* param1);
    void (*placementCopyConstruct)(void* param1, const void* param2);
    void (*destruct)(void* param1);
    void (*assign)(void* param1, const void* param2);
};

class IType
{
  public:
    void PlacementConstruct(void* p_Object, const void* p_Source) const
    {
        m_pTypeFunctions->placementCopyConstruct(p_Object, p_Source);
    }

    void PlacementConstruct(void* p_Object) const
    {
        m_pTypeFunctions->placementConstruct(p_Object);
    }

    void Destruct(void* p_Object) const
    {
        m_pTypeFunctions->destruct(p_Object);
    }

    void Assign(void* p_Destination, const void* p_Source) const
    {
        m_pTypeFunctions->assign(p_Destination, p_Source);
    }

    void* Construct() const
    {
        IAllocator* normalAllocator = (*Globals::MemoryManager)->m_pNormalAllocator;
        void* data = normalAllocator->AllocateAligned(m_nTypeSize, m_nTypeAlignment, 0);

        m_pTypeFunctions->placementConstruct(data);

        return data;
    }

    void DestructAndFree(void* p_Object) const
    {
        m_pTypeFunctions->destruct(p_Object);

        IAllocator* normalAllocator = (*Globals::MemoryManager)->m_pNormalAllocator;

        normalAllocator->Free(p_Object);
    }

    bool IsTEntityRef() const
    {
        return m_nTypeInfoFlags & ETYPEINFOFLAG_TENTITYREF;
    }

    bool IsTResourcePtr() const
    {
        return m_nTypeInfoFlags & ETYPEINFOFLAG_TRESOURCEPTR;
    }

    bool IsClass() const
    {
        return m_nTypeInfoFlags & ETYPEINFOFLAG_CLASSTYPE;
    }

    bool IsEnum() const
    {
        return m_nTypeInfoFlags & ETYPEINFOFLAG_ENUMTYPE;
    }

    bool IsContainer() const
    {
        return m_nTypeInfoFlags & ETYPEINFOFLAG_CONTAINERTYPE;
    }

    bool IsArray() const
    {
        return m_nTypeInfoFlags & ETYPEINFOFLAG_ARRAYTYPE;
    }

    bool IsFixedArray() const
    {
        return m_nTypeInfoFlags & ETYPEINFOFLAG_FIXEDARRAYTYPE;
    }

    const STypeFunctions* m_pTypeFunctions;
    uint16_t m_nTypeSize;
    uint8_t m_nTypeAlignment;
    uint8_t m_nTypeInfoFlags;
    const char* pszTypeName;
    STypeID* typeID;
    bool (*fromString)(void* param1, const IType* type, const ZString& string);
    uint32_t (*toString)(const void* param1, const IType* type, char* param3, uint32_t param4, const ZString& string);
};

struct STypeID
{
    uint16_t flags;
    uint16_t typeNum;
    const IType* pTypeInfo;
    STypeID* pSourceType;
};

class ZTypeRegistry
{
  public:
    class ZTypeMapHashPolicy
    {
      public:
        static uint32_t GetHashCode(const ZString& p_Key)
        {
            return hash::Fnv1aLower(p_Key.ToCString(), p_Key.Length());
        }
    };

    virtual ~ZTypeRegistry() = default;

    STypeID* GetType(const ZString& p_TypeName)
    {
        auto iterator = m_typeNameMap.Find(p_TypeName);

        if (iterator != m_typeNameMap.End())
        {
            return iterator.Node().m_data.m_value;
        }

        return nullptr;
    }

    template<typename T> STypeID* GetType() const
    {
        return GetType(GlacierTypeName<T>);
    }

    PAD(0xC);
    ZMutex m_mutex;
    THashMap<ZString, STypeID*, ZTypeMapHashPolicy> m_typeNameMap;
};
