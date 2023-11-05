#pragma once

#include "ZResourcePtr.h"
#include "../Templates/TSharedPointer.h"

class ZResourceReader;

class HitmanAbsolutionSDK_API ZResourcePending
{
public:
	const ZResourcePtr& GetResource() const;
	TSharedPointer<ZResourceReader> GetResourceReader() const;

private:
	ZResourcePtr m_pResource;
	TSharedPointer<ZResourceReader> m_pResourceReader;
	int m_nPriority;
	int m_ResourceHandle;
	int m_nDataStartPosition;
};
