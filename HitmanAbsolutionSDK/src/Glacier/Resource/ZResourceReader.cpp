#include <Glacier/Resource/ZResourceReader.h>

const void* ZResourceReader::GetResourceData(unsigned int nOffset) const
{
	const unsigned char* data = m_pResourceData.GetTarget()->GetData();

	return &data[nOffset];
}

unsigned int ZResourceReader::GetResourceDataSize() const
{
	return m_nResourceDataSize;
}
