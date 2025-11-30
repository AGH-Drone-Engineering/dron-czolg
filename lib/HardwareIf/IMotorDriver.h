// lib/HardwareIf/IMotorDriver.h
#pragma once
#include <cstdint>

class IMotorDriver
{
public:
    // Wyślij surowy pakiet DShot (16 bitów -> 16 bajtów bitstream)
    virtual void sendPacket(uint16_t packet) = 0;
    
    // Wyślij throttle (0-2047) z automatyczną konwersją na pakiet DShot
    virtual void sendThrottle(uint16_t throttle, bool telemetry = false) = 0;
    
    // Wyślij komendę specjalną (0-47)
    virtual void sendCommand(uint8_t command, bool telemetry = false) = 0;
    
    virtual ~IMotorDriver() {}
};