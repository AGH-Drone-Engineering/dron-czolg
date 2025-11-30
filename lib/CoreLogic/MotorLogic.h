#pragma once
#include "IMotorDriver.h"
#include "config.h"

class MotorLogic
{
    IMotorDriver &driver;                                  // DI: Wstrzyknięcie drivera
    uint16_t createPacket(uint16_t value, bool telemetry); // Funkcja pomocnicza

public:
    MotorLogic(IMotorDriver &d) : driver(d) {}
    void setThrottle(float throttle); // Konwertuj i wyślij
};
