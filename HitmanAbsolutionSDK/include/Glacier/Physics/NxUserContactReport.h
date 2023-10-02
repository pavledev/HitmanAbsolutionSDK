#pragma once

class NxContactPair;

class NxUserContactReport
{
public:
	virtual void onContactNotify(NxContactPair& nxContactPair, unsigned int param2) = 0;
	virtual ~NxUserContactReport() = default;
};
