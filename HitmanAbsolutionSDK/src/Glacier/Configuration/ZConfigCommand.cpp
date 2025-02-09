#include <Glacier/Configuration/ZConfigCommand.h>

#include <Function.h>
#include <Global.h>

ZConfigCommand* ZConfigCommand::First()
{
	return Function::CallAndReturn<ZConfigCommand*>(BaseAddress + 0xD86D0);
}

ZConfigCommand* ZConfigCommand::Next() const
{
	return m_pNext;
}

void ZConfigCommand::ExecuteCommand(const ZString& pCommandName, const char* argv)
{
	Function::Call<const ZString&, const char*>(BaseAddress + 0x52EF10, pCommandName, argv);
}

ZString ZConfigCommand::Name() const
{
	return m_pszName;
}
