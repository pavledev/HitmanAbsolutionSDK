#pragma once

template <unsigned int T>
class alignas(T) TAlignedType
{
public:
    char dummy;
};
