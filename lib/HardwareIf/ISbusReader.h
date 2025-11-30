#pragma once
#include <cstdint>

// Status odczytu SBUS
enum class SbusStatus
{
    OK = 0,
    LOST_FRAME = 1,
    FAILSAFE = 2
};

class ISbusReader
{
public:
    virtual bool readChannels() = 0;
    virtual int16_t getChannel(int ch) const = 0;
    virtual SbusStatus getStatus() const = 0;      // Status: OK, LOST_FRAME, FAILSAFE
    virtual bool isFailsafe() const = 0;           // Czy jest failsafe
    virtual bool isLostFrame() const = 0;          // Czy utracono ramkę
    virtual ~ISbusReader() {}
};