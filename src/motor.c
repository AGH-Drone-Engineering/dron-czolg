#include <motor.h>


void setMotorPWM(int pin, int value) {
    // Clamp value to allowed range
    if (value < ESC_INPUT_MIN) value = ESC_INPUT_MIN;
    if (value > ESC_INPUT_MAX) value = ESC_INPUT_MAX;

    analogWrite(pin, value);
}

void setVehiclePWM(const motors_pwm_s * motors_pwm) {
    // TODO
    // Normalize inputs
    // Set motor PWM
    setMotorPWM(MOTOR_PIN_BL, motors_pwm->bl);
    setMotorPWM(MOTOR_PIN_BR, motors_pwm->br);
    setMotorPWM(MOTOR_PIN_FL, motors_pwm->fl);
    setMotorPWM(MOTOR_PIN_FR, motors_pwm->fr);
    //tank
    setMotorPWM(MOTOR_PIN_TL, motors_pwm->tl);
    setMotorPWM(MOTOR_PIN_TR, motors_pwm->tr);
}