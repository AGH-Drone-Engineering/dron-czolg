#include <motor.h>

#include "DShot.h"

DShot motor0(MOTOR_SERIAL_FL, DShotType::DShot600);
DShot motor1(MOTOR_SERIAL_FR, DShotType::DShot600);
DShot motor2(MOTOR_SERIAL_BL, DShotType::DShot600);
DShot motor3(MOTOR_SERIAL_BR, DShotType::DShot600);
// DShot motor4(MOTOR_SERIAL_TL, DShotType::DShot600);
// DShot motor5(MOTOR_SERIAL_TR, DShotType::DShot600);

void setMotorPWM(int pin, int value) {
    // Clamp value to allowed range
    if (value < ESC_INPUT_MIN) value = ESC_INPUT_MIN;
    if (value > ESC_INPUT_MAX) value = ESC_INPUT_MAX;

    analogWrite(pin, value);
}

void setVehiclePWM(const motors_pwm_s* motors_pwm) {
    // TODO
    // Normalize inputs
    // Set motor PWM
    // setMotorPWM(MOTOR_PIN_BL, motors_pwm->bl);
    // setMotorPWM(MOTOR_PIN_BR, motors_pwm->br);
    // setMotorPWM(MOTOR_PIN_FL, motors_pwm->fl);
    // setMotorPWM(MOTOR_PIN_FR, motors_pwm->fr);
    // tank
    // setMotorPWM(MOTOR_PIN_TL, motors_pwm->tl);
    // setMotorPWM(MOTOR_PIN_TR, motors_pwm->tr);
}