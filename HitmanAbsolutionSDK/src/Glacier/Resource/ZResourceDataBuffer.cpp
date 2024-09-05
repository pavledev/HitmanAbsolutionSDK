#include <cstring>

#include <Glacier/Resource/ZResourceDataBuffer.h>

#include <Function.h>
#include <Global.h>

unsigned char* ZResourceDataBuffer::GetData() const
{
	return m_pData;
}

unsigned int ZResourceDataBuffer::GetSize() const
{
	return m_nSize;
}

void ZResourceDataBuffer::Insert(unsigned char* pData, unsigned int size)
{
	memcpy(&m_pData[m_nSize], pData, size);

	m_nSize += size;
}

TSharedPointer<ZResourceDataBuffer> ZResourceDataBuffer::Create(void* data, unsigned int capacity)
{
	return Function::CallRVOAndReturn<TSharedPointer<ZResourceDataBuffer>, void*, unsigned int>(BaseAddress + 0x25F990, data, capacity);
}

void ZResourceDataBuffer::Clear()
{
	m_nSize = 0;
}
