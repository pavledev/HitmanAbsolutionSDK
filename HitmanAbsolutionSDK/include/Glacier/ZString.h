#pragma once

#include <string_view>

class ZString
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
	bool operator==(const ZString& other) const;
	bool StartsWith(const ZString& other) const;
	bool IsAllocated() const;
	std::string_view ToStringView() const;
	operator std::string_view() const;
	static ZString CopyFrom(const ZString& other);
	
private:
	void Allocate(const char* str, size_t size);

	unsigned int m_length;
	const char* m_chars;
};
