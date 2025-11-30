#pragma once
#include "ISbusReader.h"
#include "config.h"

class SbusParser
{
    ISbusReader &reader; // DI: Wstrzyknięcie interfejsu
    float data[6];       // Parsed data

public:
    SbusParser(ISbusReader &r) : reader(r) {}
    bool parse(); // Parsuje kanały na throttle/yaw itp.
    float getThrottle() const { return data[0]; }
    float getYaw() const { return data[1]; }
    float getPitch() const { return data[2]; }
    float getRoll() const { return data[3]; }
    float getArmStatus() const { return data[4]; }
    float getMode() const { return data[5]; }
};
