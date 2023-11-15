#include <Glacier/Resource/ZResourcePending.h>

const ZResourcePtr& ZResourcePending::GetResource() const
{
	return m_pResource;
}

const TSharedPointer<ZResourceReader>& ZResourcePending::GetResourceReader() const
{
	return m_pResourceReader;
}

TSharedPointer<ZResourceReader>& ZResourcePending::GetResourceReader()
{
	return m_pResourceReader;
}

const int ZResourcePending::GetPriority() const
{
	return m_nPriority;
}

const int ZResourcePending::GetResourceHandle() const
{
	return m_ResourceHandle;
}

const int ZResourcePending::GetDataStartPosition() const
{
	return m_nDataStartPosition;
}

void ZResourcePending::SetPriority(const int priority)
{
	m_nPriority = priority;
}

void ZResourcePending::SetResourceHandle(const int resourceHandle)
{
	m_ResourceHandle = resourceHandle;
}

void ZResourcePending::SetDataStartPosition(const int dataStartPosition)
{
	m_nDataStartPosition = dataStartPosition;
}
