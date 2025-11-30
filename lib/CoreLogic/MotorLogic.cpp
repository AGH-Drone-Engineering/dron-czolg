#include "MotorLogic.h"
#include "Utils.h"

uint16_t MotorLogic::createPacket(uint16_t value, bool telemetry)
{
    // Logika z DShot.cpp (bez UART)
    uint16_t packet = (value << 1) | telemetry;
    // Dodaj CRC itp. (skopiuj z oryginalnego)
    return packet;
}

void MotorLogic::setThrottle(float throttle)
{
    uint16_t value = constrain(throttle, 48, 2047); // Mapuj throttle
    uint16_t packet = createPacket(value, false);
    driver.sendPacket(packet); // Użyj interfejsu
}