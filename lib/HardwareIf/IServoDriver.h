#pragma once
#include <cstdint>

class IServoDriver
{
public:
    virtual void setPwm(uint8_t pin, uint16_t pwmValue) = 0; // Ustaw PWM na pinie
    virtual ~IServoDriver() {}
};