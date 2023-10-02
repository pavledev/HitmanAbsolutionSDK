#pragma once

#include <Common.h>

class ZActionMapTree;

class HitmanAbsolutionSDK_API ZInputAction
{
public:
	ZInputAction();
	ZInputAction(const char* name);
	float Analog();
	bool Digital();
	const char* GetName() const;
	char SetEnabled(bool bEnabled);

private:
	const char* m_szName;
	ZActionMapTree* m_pkMap;
	int m_iSeq;
};
