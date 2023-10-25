#include <Glacier/Resource/ZResourceID.h>

ZResourceID::ZResourceID(const ZString& rhs) : m_uri(rhs)
{
}

ZResourceID::ZResourceID(const char* rhs) : m_uri(rhs)
{
}

const ZString& ZResourceID::GetURI() const
{
	return m_uri;
}

bool ZResourceID::IsLibraryResource() const
{
	return m_uri.IndexOf("resourcelib?") != -1 || m_uri.IndexOf("securelib?") != -1;
}
