#pragma once

#include <atomic>
#include <thread>

// Guards reentry into guest code from foreign (game) threads so the host
// can guarantee no thread is executing inside this module before unloading
// it.
class ReloadGate
{
  public:
    bool TryEnter()
    {
        ++m_Inflight;

        if (m_Reloading)
        {
            --m_Inflight;
            return false;
        }

        return true;
    }

    void Leave()
    {
        --m_Inflight;
    }

    // Called from the host's loader thread; spinning is fine there.
    void BeginReload()
    {
        m_Reloading = true;

        while (m_Inflight != 0)
        {
            std::this_thread::yield();
        }
    }

    void Reset()
    {
        m_Reloading = false;
    }

  private:
    std::atomic<bool> m_Reloading{ false };
    std::atomic<int32_t> m_Inflight{ 0 };
};
