#pragma once

#include <HardwareSerial.h>
#include <math.h>
#include <Arduino.h>

constexpr uint8_t DSHOT_ZERO_BYTE = 0b1111'1000;
constexpr uint8_t DSHOT_ONE_BYTE = 0b1000'0000;

enum DShotType
{
    DShot150,
    DShot300,
    DShot600,
    // DShot1200,
};

class DShot
{
private:
    HardwareSerial *uart;
    void sendPackage(uint16_t package);
    int16_t convertThrottle(float throttle);

public:
    DShot(HardwareSerial *uart, DShotType type);
    virtual ~DShot() = default;

    void sendThrottle(int16_t throttle, bool telemetry = false);
    void sendCommand(uint8_t command, bool telemetry = false);
};