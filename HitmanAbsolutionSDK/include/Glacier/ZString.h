#pragma once

#include <string_view>

#include <Common.h>

#include "ZSerializer.h"
#include "ZMemory.h"

#include <Globals.h>
#include <Hash.h>

class ZString
{
  public:
    ZString() : m_length(0x80000000), m_chars(const_cast<char*>("")) {}

    ZString(std::string_view str) : m_length(static_cast<uint32_t>(str.size()) | 0x80000000), m_chars(str.data()) {}

    ZString(const std::string& str) : ZString()
    {
        Allocate(str.c_str(), str.size());
    }

    ZString(const char* str) : m_length(static_cast<uint32_t>(std::strlen(str)) | 0x80000000), m_chars(str) {}

    ZString(const char* str, size_t size) : m_length(static_cast<uint32_t>(size) | 0x80000000), m_chars(str) {}

    ZString(const ZString& other)
    {
        if (other.IsAllocated())
        {
            Allocate(other.ToCString(), other.Length());
        }
        else
        {
            m_length = other.m_length;
            m_chars = other.m_chars;
        }
    }

    ZString& operator=(const ZString& other)
    {
        if (this != &other)
        {
            if (other.IsAllocated())
            {
                Allocate(other.ToCString(), other.Length());
            }
            else
            {
                m_length = other.m_length;
                m_chars = other.m_chars;
            }
        }

        return *this;
    }

    ~ZString()
    {
        /*if (IsAllocated())
        {
            Free();
        }*/
    }

    uint32_t Length() const
    {
        return m_length & 0x3FFFFFFF;
    }

    const char* ToCString() const
    {
        return m_chars;
    }

    bool operator==(const ZString& other) const
    {
        if (Length() != other.Length())
        {
            return false;
        }

        return strncmp(m_chars, other.m_chars, Length()) == 0;
    }

    bool StartsWith(const ZString& other) const
    {
        if (Length() != other.Length())
        {
            return false;
        }

        return strncmp(m_chars, other.m_chars, other.Length()) == 0;
    }

    bool IsAllocated() const
    {
        return (m_length & 0xC0000000) == 0;
    }

    std::string_view ToStringView() const
    {
        return std::string_view(m_chars, Length());
    }

    operator std::string_view() const
    {
        return ToStringView();
    }

    ZString CopyFrom(const ZString& other)
    {
        ZString string;

        string.Allocate(other.m_chars, other.Length());

        return string;
    }

    void Allocate(const char* str, size_t size)
    {
        IAllocator* normalAllocator = (*Globals::MemoryManager)->m_pNormalAllocator;
        char* chars = reinterpret_cast<char*>(normalAllocator->Allocate(size + 1, 0));

        strncpy_s(chars, size + 1, str, size + 1);

        m_length = static_cast<uint32_t>(size);
        m_chars = chars;
    }

    void Free()
    {
        IAllocator* normalAllocator = (*Globals::MemoryManager)->m_pNormalAllocator;

        normalAllocator->Free(const_cast<char*>(m_chars));

        m_chars = nullptr;
    }

    bool IsEmpty() const
    {
        return Length() == 0;
    }

    int IndexOf(const char* rhs) const
    {
        const char* foundPtr = strstr(m_chars, rhs);

        if (foundPtr)
        {
            return static_cast<int>(foundPtr - m_chars);
        }

        return -1;
    }

    uint32_t GetHashCode() const
    {
        return hash::Fnv1a(ToCString(), Length());
    }

    void SerializeToMemory(ZBinarySerializer& binarySerializer, const uint32_t offset)
    {
        binarySerializer.SetLayoutPointer(binarySerializer.GetAlignedLayoutPointer(binarySerializer.GetLayoutPointer(), alignof(ZString)));

        uint32_t length = Length() + 1;

        uint32_t lengthOffset = offset + offsetof(ZString, m_length);
        uint32_t charsOffset = offset + offsetof(ZString, m_chars);
        bool useMaxAlignment = true;

        if (binarySerializer.GetMaxAlignment() >= 8)
        {
            useMaxAlignment = false;
        }

        uint32_t charsOffset2 = binarySerializer.ReserveLayoutFor(length, sizeof(char), 1, 4, useMaxAlignment);

        binarySerializer.WriteToMemory(&length, sizeof(length), charsOffset2 - 4);
        binarySerializer.WriteToMemory(m_chars, length, charsOffset2);

        uint32_t length2 = Length() | 0x40000000;

        binarySerializer.WriteToMemory(&length2, sizeof(uint32_t), lengthOffset);
        binarySerializer.WriteToMemory(&charsOffset2, sizeof(uint32_t), charsOffset);

        binarySerializer.RecordOffsetForRebasing(charsOffset);
    }

  private:
    uint32_t m_length;
    const char* m_chars;
};

template<> struct fmt::formatter<ZString>
{
    constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator
    {
        return ctx.begin();
    }

    auto format(const ZString& r, format_context& ctx) const -> format_context::iterator
    {
        return fmt::format_to(ctx.out(), "{}", r.ToStringView());
    }
};

inline std::ostream& operator<<(std::ostream& p_Stream, const ZString& p_String)
{
    return p_Stream.write(p_String.ToCString(), p_String.Length());
}

inline ZString operator""_zs(const char* p_String, size_t p_Size)
{
    return { std::string_view(p_String, p_Size) };
}
