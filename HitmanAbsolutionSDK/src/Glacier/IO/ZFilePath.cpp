#include <Glacier/IO/ZFilePath.h>

ZFilePath::ZFilePath(const ZString& rhs) : m_path(rhs)
{
}

ZString ZFilePath::ToString() const
{
    return m_path.ToString();
}
