#pragma once

class ZGameTime
{
public:
    long long GetTicks() const;

private:
    long long m_nTicks;
};
