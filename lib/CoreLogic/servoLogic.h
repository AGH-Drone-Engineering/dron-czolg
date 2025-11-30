// lib/CoreLogic/ServoLogic.h
#pragma once
#include "IServoDriver.h"
#include "config.h"

class ServoLogic
{
    IServoDriver &driver;      // DI: Wstrzyknięcie drivera
    uint8_t leftPin, rightPin; // Piny z config
    
    // Predefiniowane kąty dla trybów (z config.h)
    static constexpr int DRONE_ANGLE = SERVO_ANGLE_COPTER;
    static constexpr int TANK_ANGLE = SERVO_ANGLE_TANK;
    
    // Stan
    bool attached = false;

public:
    ServoLogic(IServoDriver &d, uint8_t left, uint8_t right) 
        : driver(d), leftPin(left), rightPin(right) {}
    
    // Attach/Detach serw
    void attach();
    void detach();
    bool isAttached() const { return attached; }
    
    // Ustaw kąt dla obu serw (0-180)
    void setAngle(int angle);
    
    // Ustaw kąt z mapowaniem z float (-90 do 90 -> 0 do 180)
    void setAngleFloat(float angle);
    
    // Predefiniowane tryby
    void setTankMode();
    void setCopterMode();
    
    // Bezpośrednie ustawienie PWM (mikro sekundy)
    void setMicroseconds(int us);
};
