#pragma once

class ZBufferBlock
{
public:
	unsigned char* GetData() const;
	const unsigned int GetReadSize() const;

private:
	unsigned char* pData;
	unsigned int nPosition;
	unsigned int nReadSize;
	volatile bool m_bLoaded;
};
