#pragma once

#include "../ZSharedPointerTarget.h"
#include "../Templates/TSharedPointer.h"

class ZResourceDataBuffer : public ZSharedPointerTarget
{
public:
	virtual ~ZResourceDataBuffer() = 0;

	unsigned char* GetData() const;

private:
	unsigned char* m_pData;
	unsigned int m_nSize;
	unsigned int m_nCapacity;
	bool m_bOwnsDataPtr;
};
