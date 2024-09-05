#pragma once

#include <algorithm>

#include "../Memory/ZMemoryManager.h"
#include "../Memory/IAllocator.h"
#include "../Serializer/ZBinarySerializer.h"

#include <Global.h>

template <typename T>
class TArray
{
public:
    TArray()
    {
        m_pStart = nullptr;
        m_pEnd = nullptr;
        m_pLast = nullptr;
    }

    TArray(const size_t initialSize)
    {
        IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();

        m_pStart = static_cast<T*>(normalAllocator->AllocateAligned(sizeof(T) * initialSize, alignof(T), 0));

        for (size_t i = 0; i < initialSize; ++i)
        {
            new (m_pStart + i) T();
        }

        m_pEnd = m_pStart + initialSize;
        m_pLast = m_pEnd;
    }

    TArray(const TArray& other)
    {
        IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();
        const size_t size = other.Size();
        m_pStart = static_cast<T*>(normalAllocator->AllocateAligned(sizeof(T) * size, alignof(T), 0));

        for (size_t i = 0; i < size; ++i)
        {
            new (m_pStart + i) T(other.m_pStart[i]);
        }

        m_pEnd = m_pStart + size;
        m_pLast = m_pEnd;
    }

    TArray(TArray&& other) noexcept : m_pStart(other.m_pStart), m_pEnd(other.m_pEnd), m_pLast(other.m_pLast)
    {
        other.m_pStart = nullptr;
        other.m_pEnd = nullptr;
        other.m_pLast = nullptr;
    }

    ~TArray()
    {
        for (T* p = m_pStart; p != m_pEnd; ++p)
        {
            p->~T();
        }

        IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();

        normalAllocator->Free(m_pStart);

        m_pStart = nullptr;
        m_pEnd = nullptr;
        m_pLast = nullptr;
    }

    TArray& operator=(const TArray& other)
    {
        if (this != &other)
        {
            for (T* p = m_pStart; p != m_pEnd; ++p)
            {
                p->~T();
            }

            IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();

            normalAllocator->Free(m_pStart);

            size_t size = other.Size();
            m_pStart = static_cast<T*>(normalAllocator->AllocateAligned(sizeof(T) * size, alignof(T), 0));

            std::copy(other.m_pStart, other.m_pEnd, m_pStart);

            m_pEnd = m_pStart + size;
            m_pLast = m_pEnd;
        }

        return *this;
    }

    TArray& operator=(TArray&& other) noexcept
    {
        if (this != &other)
        {
            for (T* p = m_pStart; p != m_pEnd; ++p)
            {
                p->~T();
            }

            IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();

            normalAllocator->Free(m_pStart);

            m_pStart = other.m_pStart;
            m_pEnd = other.m_pEnd;
            m_pLast = other.m_pLast;

            other.m_pStart = nullptr;
            other.m_pEnd = nullptr;
            other.m_pLast = nullptr;
        }

        return *this;
    }

    const T* GetStart() const
    {
        return m_pStart;
    }

    T* GetStart()
    {
        return m_pStart;
    }

    const T* GetEnd() const
    {
        return m_pEnd;
    }

    T* GetEnd()
    {
        return m_pEnd;
    }

    const T* GetLast() const
    {
        return m_pLast;
    }

    T* GetLast()
    {
        return m_pLast;
    }

    void SetStart(T* pStart)
    {
        m_pStart = pStart;
    }

    void SetEnd(T* pEnd)
    {
        m_pEnd = pEnd;
    }

    void Reserve(const size_t capacity)
    {
        if (capacity == 0)
        {
            return;
        }

        if (capacity <= Capacity())
        {
            return;
        }

        IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();
        T* newStart = static_cast<T*>(normalAllocator->AllocateAligned(sizeof(T) * capacity, alignof(T), 0));
        size_t currentSize = Size();

        for (size_t i = 0; i < currentSize; ++i)
        {
            new(newStart + i) T(std::move(m_pStart[i]));
        }

        for (size_t i = 0; i < currentSize; ++i)
        {
            m_pStart[i].~T();
        }

        normalAllocator->Free(m_pStart);

        m_pStart = newStart;
        m_pEnd = m_pStart + currentSize;
        m_pLast = m_pStart + capacity;
    }

    void Resize(const size_t newSize)
    {
        if (newSize == 0)
        {
            return;
        }

        if (newSize > Capacity())
        {
            Reserve(newSize);
        }

        if (newSize < Size())
        {
            for (T* p = m_pStart + newSize; p != m_pEnd; ++p)
            {
                p->~T();
            }
        }
        else if (newSize > Size())
        {
            for (T* p = m_pEnd; p != m_pStart + newSize; ++p)
            {
                new(p) T();
            }
        }

        m_pEnd = m_pStart + newSize;
    }

    const size_t Size() const
    {
        return m_pEnd - m_pStart;
    }

    const size_t Capacity() const
    {
        return m_pLast - m_pStart;
    }

    T& operator[](const size_t index)
    {
        return m_pStart[index];
    }

    const T& operator[](const size_t index) const
    {
        return m_pStart[index];
    }

    void Clear()
    {
        for (T* p = m_pStart; p != m_pEnd; ++p)
        {
            p->~T();
        }

        m_pEnd = m_pStart;
    }

    void PushBack(const T& element)
    {
        if (m_pEnd == m_pLast)
        {
            size_t currentSize = Size();
            size_t newSize = (currentSize == 0) ? 1 : currentSize * 2;

            Reserve(newSize);
        }

        new (m_pEnd) T(element);
        ++m_pEnd;
    }

    void RemoveAt(const size_t index)
    {
        T* element = m_pStart + index;

        element->~T();

        std::move(element + 1, m_pEnd, element);

        --m_pEnd;

        if (m_pStart != m_pEnd)
        {
            m_pEnd->~T();
        }
    }

    bool operator==(const TArray& other) const
    {
        if (Size() != other.Size())
        {
            return false;
        }

        const TArray& array = *this;

        for (unsigned int i = 0; i < Size(); ++i)
        {
            if (array[i] != other[i])
            {
                return false;
            }
        }

        return true;
    }

    void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
    {
        const unsigned int elementsCount = Size();
        unsigned int elementsStartOffset;

        if (elementsCount > 0)
        {
            const unsigned int size = sizeof(T) * elementsCount;

            elementsStartOffset = binarySerializer.ReserveLayoutFor(elementsCount, sizeof(T), alignof(T), 4);

            const unsigned int elementsEndOffset = elementsStartOffset + size;

            binarySerializer.WriteToMemory(&elementsCount, sizeof(unsigned int), elementsStartOffset - 4);

            binarySerializer.WriteToMemory(&elementsStartOffset, sizeof(unsigned int), offset);
            binarySerializer.WriteToMemory(&elementsEndOffset, sizeof(unsigned int), offset + 4);
            binarySerializer.WriteToMemory(&elementsEndOffset, sizeof(unsigned int), offset + 8);
        }

        binarySerializer.RecordOffsetForRebasing(offset);
        binarySerializer.RecordOffsetForRebasing(offset + 4);
        binarySerializer.RecordOffsetForRebasing(offset + 8);

        for (unsigned int i = 0; i < elementsCount; ++i)
        {
            const unsigned int elementOffset = elementsStartOffset + sizeof(T) * i;
            T* type = &this->operator[](i);

            if (i == 0)
            {
                binarySerializer.SetLayoutPointer(binarySerializer.GetAlignedLayoutPointer(binarySerializer.GetLayoutPointer(), alignof(TArray)));
            }

            if constexpr (std::is_class_v<T>)
            {
                type->SerializeToMemory(binarySerializer, elementOffset);
            }
            else
            {
                binarySerializer.WriteToMemory(type, sizeof(T), elementOffset);
            }
        }
    }

private:
    T* m_pStart;
    T* m_pEnd;
    T* m_pLast;
};
