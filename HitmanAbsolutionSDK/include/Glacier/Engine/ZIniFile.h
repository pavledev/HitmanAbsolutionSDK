#pragma once

#include "IIniFile.h"
#include "../Templates/TMap.h"
#include "../Templates/TArray.h"

class ZIniFileSection;

class ZIniFile : public IIniFile
{
public:
	virtual bool Load(const ZFilePath& path) = 0;
	virtual bool LoadFromString(const ZString& sSettings) = 0;
	virtual IIniFileSection* GetSection(const ZString& sName) = 0;
	virtual TEnumerator<IIniFileSection*> GetSections() = 0;
	virtual ZString GetValue(const ZString& sSectionName, const ZString& sOptionName) = 0;
	virtual void SetValue(const ZString& sSectionName, const ZString& sOptionName, const ZString& sValue) = 0;
	virtual TEnumerator<ZString> GetConsoleCmds() = 0;

	static bool LoadIniFileContent(const ZFilePath& plainTextIniFilePath, TArray<unsigned char>& aBuffer, bool bPlainText);
	bool LoadFromStringInternal(const ZString& sIniFileContent, const ZFilePath& path);

private:
	TMap<ZString, IIniFileSection*> m_sections;
	ZIniFileSection* m_pCurrentSection;
	TArray<ZString> m_ConsoleCmds;
	TArray<ZString> m_IniFilesLoaded;
};
