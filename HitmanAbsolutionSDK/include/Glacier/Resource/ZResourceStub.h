#pragma once

#include "ZResourcePtr.h"
#include "ZRuntimeResourceID.h"
#include "EResourceReferenceFlags.h"
#include "../Templates/TArray.h"
#include "EResourceStatus.h"

class IResourceInstaller;

class HitmanAbsolutionSDK_API ZResourceStub
{
public:
	virtual ~ZResourceStub() = default;

	EResourceStatus GetResourceStatus() const;
	const ZRuntimeResourceID& GetRuntimeResourceID() const;
	void* GetResourceData() const;
	unsigned int GetResourceTag() const;
	void AddRef();
	void Release();
	ZResourcePtr GetInstallDependency(unsigned int nIndex) const;
	IResourceInstaller* GetResourceInstaller() const;

private:
	struct SResourceReference
	{
		EResourceReferenceFlags m_Flags;

		union
		{
			ZResourceStub* m_pStub;
			ZRuntimeResourceID* m_pridMediaResource;
		};

		ZResourcePtr m_pResource;
	};

	ZRuntimeResourceID m_ridResource;
	unsigned char m_eResourceStatus;
	unsigned char m_RequestedLoadStatus;
	short m_nPriority;
	void* m_pResourceData;
	unsigned int m_nResourceTag;
	long m_nRef;
	long m_nHeaderRef;
	TArray<SResourceReference> m_resourceReferences;
	TArray<ZResourceStub*> m_backReferences;
};
