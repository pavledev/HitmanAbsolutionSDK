#include <Glacier/Engine/ZWin32ApplicationStub.h>

#include <Global.h>
#include <Function.h>

void ZWin32ApplicationStub::ApplyOptionOverrides(int& argc, char** argv)
{
	Function::CallMethod<ZWin32ApplicationStub*, int&, char**>(BaseAddress + 0x33BF40, this, argc, argv);
}
