#include "ServoLogic.h"
#include "Utils.h"

void ServoLogic::setAngle(float angle)
{
    uint16_t pwm = map(angle, -90, 90, 1000, 2000); // Mapowanie kąta na PWM
    driver.setPwm(leftPin, pwm);                    // Dla lewego servo
    driver.setPwm(rightPin, pwm);                   // Dla prawego (lub inny pin)
}