#pragma once
#include <Arduino.h>
#include "IServoDriver.h"

class TeensyServoDriver : public IServoDriver
{
public:
    TeensyServoDriver()
    {
        // Inicjalizacja PWM (z oryginalnego kodu)
    }
    void setPwm(uint8_t pin, uint16_t pwmValue) override
    {
        // Kod z servo_controller.cpp (PWM)
        analogWrite(pin, pwmValue);
    }
};