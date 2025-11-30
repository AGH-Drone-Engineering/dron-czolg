#pragma once
#include "IImuSensor.h"

class ImuLogic
{
    IImuSensor &sensor;                 // DI: Wstrzyknięcie sensora
    float pitch = 0, roll = 0, yaw = 0; // Obliczone kąty

public:
    ImuLogic(IImuSensor &s) : sensor(s) {}
    bool update(); // Czytaj i oblicz kąty
    float getPitch() const { return pitch; }
    float getRoll() const { return roll; }
    float getYaw() const { return yaw; }
};
