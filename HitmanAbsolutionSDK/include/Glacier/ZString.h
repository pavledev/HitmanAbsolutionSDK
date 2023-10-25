#pragma once

#include <string_view>

#include <Common.h>

class ZBinarySerializer;

class HitmanAbsolutionSDK_API ZString
{
public:
	ZString();
	ZString(std::string_view str);
	ZString(const char* str);
	ZString(const char* str, size_t size);
	ZString(const ZString& other);
	~ZString();
	unsigned int Length() const;
	const char* ToCString() const;
	void SetChars(const char* chars);
	bool operator==(const ZString& other) const;
	bool StartsWith(const ZString& other) const;
	bool IsAllocated() const;
	std::string_view ToStringView() const;
	operator std::string_view() const;
	static ZString CopyFrom(const ZString& other);
	void Allocate(const char* str, size_t size);
	void Free();
	bool IsEmpty() const;
	int IndexOf(const char* rhs) const;
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	
private:
	unsigned int m_length;
	const char* m_chars;
};
