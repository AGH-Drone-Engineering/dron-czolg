#include "ServoLogic.h"
#include "Utils.h"

void ServoLogic::attach()
{
    driver.attach(leftPin);
    driver.attach(rightPin);
    attached = true;
}

void ServoLogic::detach()
{
    driver.detach(leftPin);
    driver.detach(rightPin);
    attached = false;
}

void ServoLogic::setAngle(int angle)
{
    // Ogranicz kąt do zakresu 0-180
    angle = static_cast<int>(constrain(static_cast<float>(angle), 0.0f, 180.0f));
    
    // Biblioteka Servo mapuje kąt 0-180 na PWM 544-2400 µs
    // My używamy bezpośrednio kąta - driver powinien obsłużyć mapowanie
    driver.setAngle(leftPin, angle);
    driver.setAngle(rightPin, angle);
}

void ServoLogic::setAngleFloat(float angle)
{
    // Mapuj z zakresu -90..+90 na 0..180
    int mappedAngle = static_cast<int>(map(angle, -90.0f, 90.0f, 0.0f, 180.0f));
    setAngle(mappedAngle);
}

void ServoLogic::setTankMode()
{
    setAngle(TANK_ANGLE);
}

void ServoLogic::setCopterMode()
{
    setAngle(DRONE_ANGLE);
}

void ServoLogic::setMicroseconds(int us)
{
    // Ogranicz do typowego zakresu serw
    us = static_cast<int>(constrain(static_cast<float>(us), 500.0f, 2500.0f));
    driver.setMicroseconds(leftPin, us);
    driver.setMicroseconds(rightPin, us);
}