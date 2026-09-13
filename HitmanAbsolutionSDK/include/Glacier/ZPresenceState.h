#pragma once

#include <cstdint>

class ZPresenceState
{
  public:
    enum EntityStateFlags
    {
        ESF_STREAMED = 1,
        ESF_ACTIVATED = 2,
        ESF_EDITMODE = 4
    };

    virtual ~ZPresenceState() = 0;
    virtual void OnStreamActivateStateChanged() = 0;

    uint8_t m_nEntityState;
};
