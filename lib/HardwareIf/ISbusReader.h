#pragma once
#include <cstdint>

class ISbusReader
{
public:
    virtual bool readChannels() = 0;
    virtual int16_t getChannel(int ch) const = 0;
    virtual ~ISbusReader() {}
};