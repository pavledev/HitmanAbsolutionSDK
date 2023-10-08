#pragma once

#include <Common.h>

class ZResourceStub;

class HitmanAbsolutionSDK_API ZResourcePtr
{
public:
	ZResourceStub* GetResourceStub() const;
	void* GetRawPointer() const;

private:
	ZResourceStub* m_pResourceStub;
};
