#include <Glacier/ZStringBuilder.h>

ZStringBuilder::ZStringBuilder()
{
    m_chars.Reserve(256);
}

ZStringBuilder::ZStringBuilder(const char* pszInitial, int nLength)
{
	Initialize(pszInitial, nLength);
}

ZStringBuilder::ZStringBuilder(const ZString& sInitial)
{
    Initialize(sInitial.ToCString(), sInitial.Length() + 1);
}

void ZStringBuilder::Initialize(const char* pszData, int nLength)
{
    m_chars.Reserve(256);

    if (nLength == -1)
    {
        nLength = strlen(pszData);
    }

    if (nLength > 0)
    {
        if (nLength > m_chars.Capacity())
        {
            m_chars.Reserve(nLength);
        }

        m_chars.SetEnd(m_chars.GetEnd() + nLength);

        memcpy(m_chars.GetStart(), pszData, nLength);
    }
}

ZString ZStringBuilder::ToString() const
{
    return m_chars.GetStart();
}
