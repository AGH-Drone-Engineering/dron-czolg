#pragma once
#include <Arduino.h>
#include <Servo.h>
#include "IServoDriver.h"

/**
 * @brief Driver serw dla Teensy
 *
 * Obsługuje 2 serwa transformacji (lewe i prawe).
 * Przechowuje obiekty Servo i mapuje piny na indeksy.
 */
class TeensyServoDriver : public IServoDriver
{
    // Dwa serwa: lewe i prawe
    Servo servoLeft;
    Servo servoRight;

    uint8_t leftPin = 0;
    uint8_t rightPin = 0;
    bool leftAttached = false;
    bool rightAttached = false;

    // Zwraca wskaźnik do odpowiedniego serwa na podstawie pinu
    Servo *getServo(uint8_t pin)
    {
        if (pin == leftPin && leftAttached)
            return &servoLeft;
        if (pin == rightPin && rightAttached)
            return &servoRight;
        return nullptr;
    }

public:
    void attach(uint8_t pin) override
    {
        // Pierwsze attach = lewe serwo, drugie = prawe
        if (!leftAttached)
        {
            leftPin = pin;
            servoLeft.attach(pin);
            leftAttached = true;
        }
        else if (!rightAttached && pin != leftPin)
        {
            rightPin = pin;
            servoRight.attach(pin);
            rightAttached = true;
        }
    }

    void detach(uint8_t pin) override
    {
        if (pin == leftPin && leftAttached)
        {
            servoLeft.detach();
            leftAttached = false;
        }
        else if (pin == rightPin && rightAttached)
        {
            servoRight.detach();
            rightAttached = false;
        }
    }

    void setAngle(uint8_t pin, int angle) override
    {
        Servo *s = getServo(pin);
        if (s)
        {
            angle = constrain(angle, 0, 180);
            s->write(angle);
        }
    }

    void setMicroseconds(uint8_t pin, int us) override
    {
        Servo *s = getServo(pin);
        if (s)
        {
            us = constrain(us, 500, 2500);
            s->writeMicroseconds(us);
        }
    }

    void setPwm(uint8_t pin, uint16_t pwmValue) override
    {
        setMicroseconds(pin, pwmValue);
    }
};