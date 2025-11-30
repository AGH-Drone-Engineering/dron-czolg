#pragma once
#include <cstdint>

class IServoDriver
{
public:
    // Attach/Detach serwa do pinu
    virtual void attach(uint8_t pin) = 0;
    virtual void detach(uint8_t pin) = 0;
    
    // Ustaw kąt serwa (0-180 stopni)
    virtual void setAngle(uint8_t pin, int angle) = 0;
    
    // Ustaw pozycję serwa w mikrosekundach (typowo 500-2500 µs)
    virtual void setMicroseconds(uint8_t pin, int us) = 0;
    
    // Dla kompatybilności wstecznej
    virtual void setPwm(uint8_t pin, uint16_t pwmValue) = 0;
    
    virtual ~IServoDriver() {}
};