#pragma once

#include "ZApplicationBase.h"
#include "../Templates/TMap.h"

class HitmanAbsolutionSDK_API ZWin32ApplicationStub : public ZApplicationBase
{
public:
	virtual void Initialize(const ZString& sDefaultSettings) = 0;
	virtual void AddApplicationSpecificOptions(IIniFile* pIniFile) = 0;
	virtual bool Startup() = 0;
	virtual void Shutdown() = 0;

	void ApplyOptionOverrides(int& argc, char** argv);

public:
	PAD(0x4); //0x18
	IIniFile* m_pIniFile; //0x1C
	TMap<ZString, ZString>* m_pOptions; //0x20
	PAD(0x20);
};
