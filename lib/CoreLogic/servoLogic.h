// lib/CoreLogic/ServoLogic.h
#pragma once
#include "IServoDriver.h"
#include "config.h"

class ServoLogic
{
    IServoDriver &driver;      // DI: Wstrzyknięcie drivera
    uint8_t leftPin, rightPin; // Piny z config

public:
    ServoLogic(IServoDriver &d, uint8_t left, uint8_t right) : driver(d), leftPin(left), rightPin(right) {}
    void setAngle(float angle); // Mapuj kąt na PWM i wyślij
};
