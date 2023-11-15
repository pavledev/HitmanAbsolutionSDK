#include <Glacier/Resource/ZBufferBlock.h>

unsigned char* ZBufferBlock::GetData() const
{
	return pData;
}

const unsigned int ZBufferBlock::GetReadSize() const
{
	return nReadSize;
}
