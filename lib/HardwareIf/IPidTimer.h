#pragma once
#include <cstdint>

class IPidTimer
{
public:
    virtual uint32_t getMillis() const = 0; // Pobierz czas w ms
    virtual ~IPidTimer() {}
};