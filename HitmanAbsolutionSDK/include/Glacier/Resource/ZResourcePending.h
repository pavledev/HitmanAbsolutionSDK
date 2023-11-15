#pragma once

#include "ZResourcePtr.h"
#include "../Templates/TSharedPointer.h"
#include "ZResourceReader.h"

class HitmanAbsolutionSDK_API ZResourcePending
{
public:
	const ZResourcePtr& GetResource() const;
	const TSharedPointer<ZResourceReader>& GetResourceReader() const;
	TSharedPointer<ZResourceReader>& GetResourceReader();
	const int GetPriority() const;
	const int GetResourceHandle() const;
	const int GetDataStartPosition() const;
	void SetPriority(const int priority);
	void SetResourceHandle(const int resourceHandle);
	void SetDataStartPosition(const int dataStartPosition);

private:
	ZResourcePtr m_pResource;
	TSharedPointer<ZResourceReader> m_pResourceReader;
	int m_nPriority;
	int m_ResourceHandle;
	int m_nDataStartPosition;
};
