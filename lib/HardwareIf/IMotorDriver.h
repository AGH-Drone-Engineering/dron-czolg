// lib/HardwareIf/IMotorDriver.h
#pragma once
#include <cstdint>

class IMotorDriver
{
public:
    virtual void sendPacket(uint16_t packet) = 0; // Wyślij pakiet DShot
    virtual ~IMotorDriver() {}
};