#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Arduino.h>
#include <PWMServo.h>
#include "config.h"

class ServoController {
public:
    // Stabilne pozycje
    enum Position {
        RIDE = 0,
        FLIGHT = 1
    };

    enum Servo {
        LEFT_FRONT,
        LEFT_REAR,
        RIGHT_FRONT,
        RIGHT_REAR
    };

    ServoController();
    ~ServoController();

    // Attach serw i ustawienie na domyślną pozycję
    void begin();

    // Przełącz wszystkie serwa na jedną ze stabilnych pozycji 
    void setStablePosition(Position pos);

    // Ustaw wszystkie serva na zadany kąt.
    void setEachAngle(int angleDeg);

    // Ustaw servo na zadany kąt.
    void setServoAngle(Servo servo, int angleDeg);

    // Pobierz bieżący kąt (ostatnio ustawiony, nie z sensora)
    int getCurrentAngle(Servo servo) const;

private:
    static const int SERVO_COUNT = 4;
    PWMServo servos[4];
    int currentAngles[4];
    bool attached;
    
    int clampAngle(int angle);
};

#endif // SERVO_CONTROLLER_H
