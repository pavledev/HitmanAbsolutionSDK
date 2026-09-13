#pragma once

#include "ZMemory.h"
#include "Reflection.h"
#include "CompileReflection.h"

#include <Common.h>
#include <Globals.h>

class ZObjectRef
{
  public:
    ZObjectRef() : m_TypeID((*Globals::TypeRegistry)->GetType("void")), m_pData(nullptr) {}

    ZObjectRef(void* p_Object, STypeID* const p_TypeID) : m_TypeID(p_TypeID), m_pData(p_Object) {}

    STypeID* m_TypeID;
    void* m_pData;
};

class ZVariant : public ZObjectRef
{
  public:
    ZVariant() = default;

    ZVariant(STypeID* p_Type)
    {
        Set(p_Type);
    }

    ZVariant(STypeID* p_Type, const void* p_Data)
    {
        Set(p_Type, p_Data);
    }

    ZVariant(const ZVariantRef& p_Other);

    ZVariant(const ZVariant& p_Other)
    {
        Set(p_Other.m_TypeID, p_Other.m_pData);
    }

    ~ZVariant()
    {
        STypeID* voidTypeID = (*Globals::TypeRegistry)->GetType("void");

        if (m_TypeID != voidTypeID)
        {
            if (m_TypeID->flags == 2)
            {
                if (m_pData)
                {
                    IPageAllocator* pageAllocator = (*Globals::MemoryManager)->m_pPageAllocator;
                    IAllocator* normalAllocator = nullptr;

                    if (!pageAllocator || !(normalAllocator = pageAllocator->GetAllocator(m_pData)))
                    {
                        normalAllocator = (*Globals::MemoryManager)->m_pNormalAllocator;
                    }

                    normalAllocator->Free(m_pData);
                }
            }
            else
            {
                m_TypeID->pTypeInfo->DestructAndFree(m_pData);
            }
        }
    }

    void Set(const ZVariantRef& p_Other);

    void Set(const ZVariant& p_Other)
    {
        Set(p_Other.m_TypeID, p_Other.m_pData);
    }

    void Set(STypeID* p_Type)
    {
        IAllocator* normalAllocator = (*Globals::MemoryManager)->m_pNormalAllocator;

        m_TypeID = p_Type;
        m_pData = normalAllocator->AllocateAligned(p_Type->pTypeInfo->m_nTypeSize, p_Type->pTypeInfo->m_nTypeAlignment, 0);

        m_TypeID->pTypeInfo->PlacementConstruct(m_pData);
    }

    void Set(STypeID* p_Type, const void* p_Data)
    {
        STypeID* variantTypeID = (*Globals::TypeRegistry)->GetType("ZVariant");
        STypeID* typeID = p_Type;

        if (p_Type == variantTypeID)
        {
            const ZVariant* variant = static_cast<const ZVariant*>(p_Data);

            typeID = variant->m_TypeID;
            p_Data = variant->m_pData;
        }

        if (typeID->flags == 1)
        {
            typeID = typeID->pSourceType;
        }

        if (m_TypeID == typeID)
        {
            if (m_TypeID->typeNum != 21)
            {
                m_TypeID->pTypeInfo->Assign(m_pData, p_Data);
                return;
            }

            m_pData = const_cast<void*>(p_Data);
            return;
        }

        Clear();

        m_TypeID = typeID;

        if (typeID->typeNum == 21)
        {
            m_pData = const_cast<void*>(p_Data);
            return;
        }

        STypeID* voidTypeID = (*Globals::TypeRegistry)->GetType("void");

        if (typeID == voidTypeID)
        {
            return;
        }

        IAllocator* normalAllocator = (*Globals::MemoryManager)->m_pNormalAllocator;

        if (typeID->flags == 2)
        {
            m_pData = normalAllocator->Allocate(sizeof(void*), 0);

            *static_cast<void**>(m_pData) = *static_cast<void* const*>(p_Data);
        }
        else
        {
            m_pData = normalAllocator->Allocate(typeID->pTypeInfo->m_nTypeSize, 0);

            typeID->pTypeInfo->PlacementConstruct(m_pData, p_Data);
        }
    }

    void Allocate(STypeID* p_Type)
    {
        Clear();

        IAllocator* normalAllocator = (*Globals::MemoryManager)->m_pNormalAllocator;

        m_TypeID = p_Type;
        m_pData = normalAllocator->AllocateAligned(p_Type->pTypeInfo->m_nTypeSize, p_Type->pTypeInfo->m_nTypeAlignment, 0);

        memset(m_pData, 0, p_Type->pTypeInfo->m_nTypeSize);
    }

    void Clear()
    {
        STypeID* voidTypeID = (*Globals::TypeRegistry)->GetType("void");

        if (m_TypeID != voidTypeID)
        {
            m_TypeID->pTypeInfo->DestructAndFree(m_pData);
        }

        m_TypeID = voidTypeID;
        m_pData = nullptr;
    }

    template<typename T> T& Get()
    {
        return *static_cast<T*>(m_pData);
    }

    template<typename T> void Set(const T& p_Data)
    {
        STypeID* typeID = (*Globals::TypeRegistry)->GetType(GlacierTypeName<T>);

        Set(typeID, static_cast<const void*>(&p_Data));
    }
};

class ZVariantRef : public ZObjectRef
{
  public:
    ZVariantRef() = default;

    ZVariantRef(const ZVariant& p_Other) : ZObjectRef(p_Other.m_pData, p_Other.m_TypeID) {}

    ZVariantRef(void* p_Object, STypeID* const p_TypeID) : ZObjectRef(p_Object, p_TypeID) {}
};

inline ZVariant::ZVariant(const ZVariantRef& p_Other)
{
    Set(p_Other.m_TypeID, p_Other.m_pData);
}

inline void ZVariant::Set(const ZVariantRef& p_Other)
{
    Set(p_Other.m_TypeID, p_Other.m_pData);
}
