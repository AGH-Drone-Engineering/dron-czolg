#pragma once
#include <cstdint>

float constrain(float value, float min, float max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

float map(float value, float fromLow, float fromHigh, float toLow, float toHigh)
{
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}
