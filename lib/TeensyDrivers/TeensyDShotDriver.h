// lib/TeensyDrivers/TeensyDShotDriver.h
#pragma once
#include <Arduino.h>
#include "IMotorDriver.h"

class TeensyDShotDriver : public IMotorDriver
{
    HardwareSerial &serial;

public:
    TeensyDShotDriver(HardwareSerial &s) : serial(s) { serial.begin(115200); }
    void sendPacket(uint16_t packet) override
    {
        // Kod z DShot.cpp (UART)
        serial.write((uint8_t)(packet >> 8));
        serial.write((uint8_t)packet);
    }
};