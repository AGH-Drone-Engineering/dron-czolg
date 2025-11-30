#pragma once
#include <cstdint>

// Funkcje pomocnicze (inline aby uniknąć wielokrotnej definicji)

inline float constrain(float value, float min, float max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

inline float map(float value, float fromLow, float fromHigh, float toLow, float toHigh)
{
    // Zabezpieczenie przed dzieleniem przez zero
    if (fromHigh == fromLow)
        return toLow;
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

// Wersja dla int
inline int constrain(int value, int min, int max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}
