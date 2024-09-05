#include <Glacier/ZString.h>
#include <Glacier/Memory/ZMemoryManager.h>
#include <Glacier/Memory/IAllocator.h>
#include <Glacier/Serializer/ZBinarySerializer.h>

#include <Global.h>

ZString::ZString()
{
	m_length = 0x80000000;
	m_chars = const_cast<char*>("");
}

ZString::ZString(std::string_view str)
{
	m_length = static_cast<unsigned int>(str.size()) | 0x80000000;
	m_chars = str.data();
}

ZString::ZString(const char* str)
{
	m_length = static_cast<unsigned int>(std::strlen(str)) | 0x80000000;
	m_chars = str;
}

ZString::ZString(const char* str, size_t size)
{
	m_length = static_cast<unsigned int>(size) | 0x80000000;
	m_chars = str;
}

ZString::ZString(const ZString& other)
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

ZString& ZString::operator=(const ZString& other)
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

ZString::~ZString()
{
	/*if (IsAllocated())
	{
		Free();
	}*/
}

unsigned int ZString::Length() const
{
	return m_length & 0x3FFFFFFF;
}

const char* ZString::ToCString() const
{
	return m_chars;
}

void ZString::SetChars(const char* chars)
{
	m_chars = chars;
}

bool ZString::operator==(const ZString& other) const
{
	if (Length() != other.Length())
	{
		return false;
	}

	return strncmp(m_chars, other.m_chars, Length()) == 0;
}

bool ZString::StartsWith(const ZString& other) const
{
	if (Length() != other.Length())
	{
		return false;
	}

	return strncmp(m_chars, other.m_chars, other.Length()) == 0;
}

bool ZString::IsAllocated() const
{
	return (m_length & 0xC0000000) == 0;
}

std::string_view ZString::ToStringView() const
{
	return std::string_view(m_chars, Length());
}

ZString::operator std::string_view() const
{
	return ToStringView();
}

ZString ZString::CopyFrom(const ZString& other)
{
	ZString string;

	string.Allocate(other.m_chars, other.Length());

	return string;
}

void ZString::Allocate(const char* str, size_t size)
{
	IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();
	char* chars = reinterpret_cast<char*>(normalAllocator->Allocate(size + 1, 0));

	strncpy_s(chars, size + 1, str, size + 1);

	m_length = static_cast<uint32_t>(size);
	m_chars = chars;
}

void ZString::Free()
{
	IAllocator* normalAllocator = MemoryManager->GetNormalAllocator();

	normalAllocator->Free(const_cast<char*>(m_chars));

	m_chars = nullptr;
}

bool ZString::IsEmpty() const
{
	return Length() == 0;
}

int ZString::IndexOf(const char* rhs) const
{
	const char* foundPtr = strstr(m_chars, rhs);

	if (foundPtr)
	{
		return static_cast<int>(foundPtr - m_chars);
	}

	return -1;
}

void ZString::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	binarySerializer.SetLayoutPointer(binarySerializer.GetAlignedLayoutPointer(binarySerializer.GetLayoutPointer(), alignof(ZString)));

	unsigned int length = Length() + 1;

	unsigned int lengthOffset = offset + offsetof(ZString, m_length);
	unsigned int charsOffset = offset + offsetof(ZString, m_chars);
	bool useMaxAlignment = true;

	if (binarySerializer.GetMaxAlignment() >= 8)
	{
		useMaxAlignment = false;
	}

	unsigned int charsOffset2 = binarySerializer.ReserveLayoutFor(length, sizeof(char), 1, 4, useMaxAlignment);

	binarySerializer.WriteToMemory(&length, sizeof(length), charsOffset2 - 4);
	binarySerializer.WriteToMemory(m_chars, length, charsOffset2);

	unsigned int length2 = Length() | 0x40000000;

	binarySerializer.WriteToMemory(&length2, sizeof(unsigned int), lengthOffset);
	binarySerializer.WriteToMemory(&charsOffset2, sizeof(unsigned int), charsOffset);

	binarySerializer.RecordOffsetForRebasing(charsOffset);
}
