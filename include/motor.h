#pragma once

#include <Arduino.h>

// Motors pins
#define MOTOR_PIN_FL 1
#define MOTOR_PIN_FR 2
#define MOTOR_PIN_BL 3
#define MOTOR_PIN_BR 4
#define MOTOR_PIN_TL 5  // Tank left motor
#define MOTOR_PIN_TR 6  // Tank right motor

#define ESC_INPUT_MIN 0
#define ESC_INPUT_MAX 255

typedef struct {
    float fl, fr, bl, br, tl, tr;  // front-left, ..., tank-left, tank-right
} motors_pwm_s;

void setMotorPWM(int pin, int value);

void setVehiclePWM(const motors_pwm_s* motors_pwm);
