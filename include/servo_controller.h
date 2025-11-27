#pragma once

#include <config.h>
#include <Servo.h>

class ServoController
{
private:
    Servo servo_motor;
    int servo_pin;
    constexpr static int DRONE_ANGLE = 20;
    constexpr static int TANK_ANGLE = 180;

public:
    // Constructor: Initializes the controller with a specific pin.
    ServoController(int pin);

    // Attaches the servo to its pin. Call this in your setup().
    void attach();

    // Detaches the servo from its pin to stop sending signals.
    void detach();

    // Sets the servo position using an angle (0 to 180 degrees).
    void write_angle(int angle);

    // Sets the servo position using raw pulse width values (e.g., 1000-2000 microseconds).
    // This is useful for more precise control or for continuous rotation servos.
    void write_microseconds(int us);

    void set_servo_tank_mode();
    void set_servo_copter_mode();
};