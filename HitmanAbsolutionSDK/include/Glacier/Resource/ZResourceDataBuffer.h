#pragma once

#include "../ZSharedPointerTarget.h"
#include "../Templates/TSharedPointer.h"

class ZResourceDataBuffer : public ZSharedPointerTarget
{
public:
	virtual ~ZResourceDataBuffer() = 0;

	unsigned char* GetData() const;
	unsigned int GetSize() const;
	void Insert(unsigned char* pData, unsigned int size);
	static TSharedPointer<ZResourceDataBuffer> Create(void* data, unsigned int capacity);
	void Clear();

private:
	unsigned char* m_pData;
	unsigned int m_nSize;
	unsigned int m_nCapacity;
	bool m_bOwnsDataPtr;
};
