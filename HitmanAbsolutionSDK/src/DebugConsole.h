#pragma once

#include <thread>

#if _DEBUG
class DebugConsole
{
  public:
    DebugConsole();
    ~DebugConsole();

  public:
    void StartRedirecting();

  private:
    void StopRedirecting();

  private:
    std::thread m_InputThread;
    volatile bool m_Running;

    int32_t m_OriginalStdin;
    int32_t m_OriginalStdout;
    int32_t m_OriginalStderr;

    bool m_Redirected;
};
#endif
