#pragma once

#include "../ZSharedPointerTarget.h"
#include "../Templates/TSharedPointer.h"
#include "EResourceReferenceFlags.h"
#include "ZResourcePtr.h"
#include "ZRuntimeResourceID.h"
#include "ZResourceDataBuffer.h"

class ZResourceStub;
template <typename T> class TArray;

class HitmanAbsolutionSDK_API ZResourceReader : public ZSharedPointerTarget
{
public:
	virtual ~ZResourceReader() = default;
	virtual unsigned int GetResourceType() const = 0;
	virtual unsigned int GetNumResourceIdentifiers() const = 0;
	virtual ZRuntimeResourceID GetResourceIdentifier(unsigned int lResourceIdentifierIndex) const = 0;
	virtual EResourceReferenceFlags GetResourceFlags(unsigned int lResourceIdentifierIndex) const = 0;
	virtual void FillResourceIdentifierArray(TArray<ZRuntimeResourceID>& resourceIDs) const = 0;
	virtual ZResourcePtr GetInstallTimeDependency(unsigned int lResourceIdentifierIndex) const = 0;

	const void* GetResourceData(unsigned int nOffset) const;
	unsigned int GetResourceDataSize() const;

private:
	ZResourceStub* m_pStub;
	TSharedPointer<ZResourceDataBuffer> m_pResourceData;
	unsigned int m_nResourceDataSize;
};
