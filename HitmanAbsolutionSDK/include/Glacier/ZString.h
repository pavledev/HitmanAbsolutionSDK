#pragma once

#include <string_view>

#include <Common.h>

#include "ZSerializer.h"
#include "ZMemory.h"

#include <Globals.h>
#include <Hash.h>
#include <IModSDK.h>

class ZString
{
  public:
    struct ZImpl
    {
        uint32_t m_nAllocatedSize;
        volatile long m_nRefcount;
        ZImpl* m_pNext;
        char m_pDataStart[0]; // String data starts at the end of this struct.
    };

    ZString() : m_length(0x80000000), m_chars(const_cast<char*>("")) {}

    ZString(std::string_view str) : m_length(static_cast<uint32_t>(str.size()) | 0x80000000), m_chars(str.data()) {}

    ZString(const char* str) : m_length(static_cast<uint32_t>(std::strlen(str)) | 0x80000000), m_chars(str) {}

    ZString(const std::string& str) : ZString()
    {
        Allocate(str.c_str(), str.size());
    }

    template<size_t N> ZString(const char (&str)[N]) : ZString()
    {
        Allocate(str, N - 1);
    }

    ZString(const ZString& p_Other) : m_length(p_Other.m_length), m_chars(p_Other.m_chars)
    {
        if (IsAllocated())
        {
            _InterlockedIncrement(&GetImpl()->m_nRefcount);
        }
    }

    ZString(ZString&& p_Other) noexcept
    {
        m_length = p_Other.m_length;
        m_chars = p_Other.m_chars;

        p_Other.m_length = 0x80000000;
        p_Other.m_chars = const_cast<char*>("");
    }

    ~ZString()
    {
        SDK().FreeZString(this);
    }

    ZString& operator=(const ZString& p_Other)
    {
        if (this == &p_Other)
        {
            return *this;
        }

        if (p_Other.IsAllocated())
        {
            _InterlockedIncrement(&p_Other.GetImpl()->m_nRefcount);
        }

        SDK().FreeZString(this);

        m_length = p_Other.m_length;
        m_chars = p_Other.m_chars;

        return *this;
    }

    ZString& operator=(ZString&& p_Other) noexcept
    {
        if (this == &p_Other)
        {
            return *this;
        }

        SDK().FreeZString(this);

        m_length = p_Other.m_length;
        m_chars = p_Other.m_chars;

        p_Other.m_length = 0x80000000;
        p_Other.m_chars = const_cast<char*>("");

        return *this;
    }

    ZImpl* GetImpl() const
    {
        return reinterpret_cast<ZImpl*>(const_cast<char*>(m_chars)) - 1;
    }

    uint32_t Length() const
    {
        return m_length & 0x3FFFFFFF;
    }

    const char* ToCString() const
    {
        return m_chars;
    }

    bool operator==(const ZString& p_Other) const
    {
        if (Length() != p_Other.Length())
        {
            return false;
        }

        return strncmp(m_chars, p_Other.m_chars, Length()) == 0;
    }

    bool StartsWith(const ZString& p_Other) const
    {
        if (Length() != p_Other.Length())
        {
            return false;
        }

        return strncmp(m_chars, p_Other.m_chars, p_Other.Length()) == 0;
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

    void Allocate(const char* p_Str, size_t p_Size)
    {
        SDK().AllocateZString(this, p_Str, p_Size);
    }

    bool IsEmpty() const
    {
        return Length() == 0;
    }

    int IndexOf(const char* p_Other) const
    {
        const char* foundPtr = strstr(m_chars, p_Other);

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

  public:
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
