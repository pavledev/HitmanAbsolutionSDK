#include <Glacier/Resource/ZRuntimeResourceID.h>
#include <Glacier/Resource/LocalResourceIDsResolver.h>

#include <Registry/ResourceIDRegistry.h>
#include <Global.h>
#include <Hash.h>
#include <Utility/StringUtility.h>

ZRuntimeResourceID::ZRuntimeResourceID()
{
	m_IDHigh = 0;
	m_IDLow = 0;
}

ZRuntimeResourceID::ZRuntimeResourceID(unsigned long long runtimeResourceID)
{
	m_IDHigh = runtimeResourceID >> 32;
	m_IDLow = static_cast<unsigned int>(runtimeResourceID);
}

ZRuntimeResourceID::ZRuntimeResourceID(unsigned int idHigh, unsigned int idLow)
{
	m_IDHigh = idHigh;
	m_IDLow = idLow;
}

ZRuntimeResourceID::operator long long() const
{
	return GetID();
}

unsigned long long ZRuntimeResourceID::GetID() const
{
	return (static_cast<unsigned long long>(m_IDHigh) << 32) | m_IDLow;
}

bool ZRuntimeResourceID::operator==(const ZRuntimeResourceID& other) const
{
	return GetID() == other.GetID();
}

bool ZRuntimeResourceID::operator!=(const ZRuntimeResourceID& other) const
{
	return GetID() != other.GetID();
}

ZRuntimeResourceID ZRuntimeResourceID::QueryRuntimeResourceID(const ZResourceID& idResource)
{
    ZRuntimeResourceID runtimeResourceID = ResourceIDRegistry::GetInstance().GetRuntimeResourceID(idResource.GetURI().ToCString());

    if (runtimeResourceID.GetID() == -1)
    {
        const std::string resourceID = StringUtility::ToLowerCase(idResource.GetURI().ToCString());

        runtimeResourceID = Hash::GetMD5(resourceID);
    }

    if (LocalResourceIDsResolverSingleton && !runtimeResourceID.IsLibraryResource())
    {
        (*LocalResourceIDsResolverSingleton)->RecordMapping(runtimeResourceID, idResource);
    }

    return runtimeResourceID;
}

ZResourceID ZRuntimeResourceID::QueryResourceID(const ZRuntimeResourceID& ridResource)
{
	return ResourceIDRegistry::GetInstance().GetResourceID(ridResource);
}

ZRuntimeResourceID ZRuntimeResourceID::Create(unsigned long long nResource)
{
    ZRuntimeResourceID result;

    result.m_IDHigh = nResource >> 32;
    result.m_IDLow = static_cast<unsigned int>(nResource);

    return result;
}

ZRuntimeResourceID ZRuntimeResourceID::CreateLibraryResourceID(ZRuntimeResourceID ridLibrary, int indexInLibrary)
{
    ZRuntimeResourceID result;

    result.m_IDHigh = indexInLibrary & 0xFFFFFF | 0x80000000;
    result.m_IDLow = ridLibrary.m_IDLow;

    return result;
}

bool ZRuntimeResourceID::IsLibraryResource() const
{
    return m_IDHigh >> 31;
}

bool ZRuntimeResourceID::IsLibrary() const
{
    return (m_IDHigh >> 30) & 1;
}

ZRuntimeResourceID ZRuntimeResourceID::GetLibraryRuntimeResourceID() const
{
    ZRuntimeResourceID result;

    result.m_IDHigh = 0x40000000;
    result.m_IDLow = m_IDLow;

    return result;
}

int ZRuntimeResourceID::GetIndexInLibrary() const
{
    return m_IDHigh & 0xFFFFFF;
}

const unsigned int ZRuntimeResourceID::GetIDHigh() const
{
    return m_IDHigh;
}

const unsigned int ZRuntimeResourceID::GetIDLow() const
{
    return m_IDLow;
}
