#include <Glacier/IO/ZPath.h>

ZPath::ZPath(const ZString& rhs) : m_value(rhs)
{
}

ZString ZPath::ToString() const
{
    return m_value.ToString();
}
