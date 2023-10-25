#include <Windows.h>

#include <Glacier/ZThread.h>

void ZThread::Sleep(unsigned int nMsecs)
{
    if (nMsecs >= 20)
    {
        ::Sleep(nMsecs);
    }
    else
    {
        timeBeginPeriod(1);
        ::Sleep(nMsecs);
        timeEndPeriod(1);
    }
}
