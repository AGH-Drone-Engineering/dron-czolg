#pragma once

#include <Arduino.h>

// we would use 1 esc for 4 drone engines and 2 other controllers for tank tracks
// motor serials are:
#define MOTOR_SERIAL_FL &Serial1
#define MOTOR_SERIAL_FR &Serial2
#define MOTOR_SERIAL_BL &Serial3
#define MOTOR_SERIAL_BR &Serial4

// idk how it would be connected
// #define MOTOR_SERIAL_TL &Serial5  // Tank left motor
// #define MOTOR_SERIAL_TR &Serial6  // Tank right motor

#define ESC_INPUT_MIN 0
#define ESC_INPUT_MAX 255

typedef struct {
    float fl, fr, bl, br, tl, tr;  // front-left, ..., tank-left, tank-right
} motors_pwm_s;

void setMotorPWM(int pin, int value);

void setVehiclePWM(const motors_pwm_s* motors_pwm);
