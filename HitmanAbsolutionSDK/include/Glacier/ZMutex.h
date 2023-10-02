#pragma once

class ZMutex
{
private:
	unsigned int m_impl[6];
	unsigned int m_nUniqueID;
};
