#pragma once

#include "../IComponentInterface.h"
#include "../IO/ZFilePath.h"

class IIniFileSection;
template <typename T> class TEnumerator;
class ZString;

class IIniFile : public IComponentInterface
{
public:
	virtual ~IIniFile() = 0;
	virtual bool Load(const ZFilePath& filePath) = 0;
	virtual bool LoadFromString(const ZString& string) = 0;
	virtual IIniFileSection* GetSection(const ZString& string) = 0;
	virtual TEnumerator<IIniFileSection*> GetSections() = 0;
	virtual ZString GetValue(const ZString& string, const ZString& string2) = 0;
	virtual void SetValue(const ZString& string, const ZString& string2, const ZString& string3) = 0;
	virtual TEnumerator<ZString> GetConsoleCmds() = 0;
};
