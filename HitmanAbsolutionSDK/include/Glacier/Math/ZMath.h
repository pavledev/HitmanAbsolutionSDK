#pragma once

#include <algorithm>

class ZMath
{
public:
    static float MapRange01(float fSource, float fSourceRangeMin, float fSourceRangeMax)
    {
        if (fSourceRangeMin > fSourceRangeMax)
        {
            std::swap(fSourceRangeMin, fSourceRangeMax);
        }

        if (fSource > fSourceRangeMax)
        {
            fSource = fSourceRangeMax;
        }
        else if (fSource < fSourceRangeMin)
        {
            fSource = fSourceRangeMin;
        }

        return (fSource - fSourceRangeMin) / (fSourceRangeMax - fSourceRangeMin);
    }
};
