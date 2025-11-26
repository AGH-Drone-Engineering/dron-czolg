#include "servo_controller.h"

// Constructor implementation.
// It takes the pin number and stores it in the private member variable.
ServoController::ServoController(int pin) : servo_pin(pin)
{
    // The constructor's body is empty because we only need to initialize the pin.
}

// Attach the servo to the pin that was specified in the constructor.
void ServoController::attach()
{
    servo_motor.attach(servo_pin);
}

// Detach the servo from its pin.
void ServoController::detach()
{
    servo_motor.detach();
}

// Write an angle to the servo. The Arduino Servo library handles the conversion
// from angle (0-180) to the correct pulse width.
void ServoController::write_angle(int angle)
{
    servo_motor.write(angle);
}

void ServoController::set_servo_tank_mode()
{
    // Example pulse width for tank mode
    this->write_angle(TANK_ANGLE);
}

void ServoController::set_servo_copter_mode()
{
    // Example pulse width for copter mode
    this->write_angle(DRONE_ANGLE);
}
