#pragma once

class ZPresenceState
{
public:
	virtual ~ZPresenceState() = 0;
	virtual void OnStreamActivateStateChanged() = 0;

private:
	enum EntityStateFlags
	{
		ESF_STREAMED = 1,
		ESF_ACTIVATED = 2,
		ESF_EDITMODE = 4
	};

	unsigned char m_nEntityState;
};
