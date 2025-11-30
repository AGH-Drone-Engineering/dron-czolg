#pragma once
#include <cstdint>

// Przeciążone constrain (jak w Arduino) – obsługuje mieszane typy
long constrain(long value, long min, long max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

float constrain(float value, float min, float max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

int constrain(int value, int min, int max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

int16_t constrain(int16_t value, int16_t min, int16_t max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

// Przeciążone map (jak w Arduino) – obsługuje mieszane typy
long map(long value, long fromLow, long fromHigh, long toLow, long toHigh)
{
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

float map(float value, float fromLow, float fromHigh, float toLow, float toHigh)
{
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

int map(int value, int fromLow, int fromHigh, int toLow, int toHigh)
{
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}
int16_t map(int16_t value, int16_t fromLow, int16_t fromHigh, int16_t toLow, int16_t toHigh)
{
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}