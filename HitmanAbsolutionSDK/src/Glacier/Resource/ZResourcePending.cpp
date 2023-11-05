#include <Glacier/Resource/ZResourcePending.h>

const ZResourcePtr& ZResourcePending::GetResource() const
{
	return m_pResource;
}

TSharedPointer<ZResourceReader> ZResourcePending::GetResourceReader() const
{
	return m_pResourceReader;
}
